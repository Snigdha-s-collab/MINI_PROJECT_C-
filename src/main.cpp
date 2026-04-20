#include "httplib.h"
#include "nlohmann/json.hpp"
#include "Engine.h"
#include "Models.h"
#include <iostream>

using json = nlohmann::json;

// Global Data State
vector<Student> students;
vector<Company> companies;
vector<Internship> internships;
vector<Application> applications;
vector<Notification> notifications;
FileManager fm;

// Helper to save all
void saveAll() {
    fm.saveAll(students, companies, internships, applications, notifications);
}

// Helpers
int nextStudentID() { int m=0; for(auto&s:students) m=max(m,s.getId()); return m+1; }
int nextAppID() { int m=0; for(auto&a:applications) m=max(m,a.getId()); return m+1; }
Student* findStudent(int id) { for(auto&s:students) if(s.getId()==id) return &s; return nullptr; }
Company* findCompany(int id) { for(auto&c:companies) if(c.getId()==id) return &c; return nullptr; }
Internship* findInternship(int id) { for(auto&i:internships) if(i.getId()==id) return &i; return nullptr; }

int main(int argc, char** argv) {
    // Load Data
    students = fm.loadStudents();
    companies = fm.loadCompanies();
    internships = fm.loadInternships();
    applications = fm.loadApplications();
    notifications = fm.loadNotifications();
    for(auto&i:internships) i.autoClose();

    // Check if running in standard CLI mode (Not asked by API logic but preserved for legacy)
    if(argc == 1) {
        std::cout << "Starting Web API Server on http://localhost:8080\n";
        std::cout << "Make sure to open your browser to view the application.\n";
    }

    httplib::Server svr;

    // CORS Middleware
    svr.set_post_routing_handler([](const auto& req, auto& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    });
    svr.Options(R"(.*)", [](const auto& req, auto& res) {
        res.status = 200;
    });

    // Mount Static Files
    svr.set_mount_point("/", "./public");

    // ==========================================
    // API: Register Student
    // ==========================================
    svr.Post("/api/register/student", [](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            string email = body["email"];
            string pwd = body["password"];
            for(auto&s:students) {
                if(s.getEmail() == email) {
                    res.set_content(json({{"success", false}, {"message", "Email already registered"}}).dump(), "application/json");
                    return;
                }
            }
            vector<string> skills = StrUtil::split(body["skills"], ',');
            Student s(nextStudentID(), body["name"], email, StrUtil::hashPassword(pwd), 
                      body["cgpa"], skills, body["resume"]);
            students.push_back(s);
            saveAll();
            res.set_content(json({{"success", true}}).dump(), "application/json");
        } catch(std::exception& e) {
            res.set_content(json({{"success", false}, {"message", "Invalid Request"}}).dump(), "application/json");
        }
    });

    // ==========================================
    // API: Login Student
    // ==========================================
    svr.Post("/api/login/student", [](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            string email = body["email"];
            string pwd = body["password"];
            for(auto&s:students) {
                if(s.getEmail() == email && s.checkPassword(pwd)) {
                    res.set_content(json({{"success", true}, {"data", {{"id", s.getId()}, {"name", s.getName()}}}}).dump(), "application/json");
                    return;
                }
            }
            res.set_content(json({{"success", false}, {"message", "Invalid email or password"}}).dump(), "application/json");
        } catch(std::exception& e) {
            res.set_content(json({{"success", false}, {"message", "Bad Request"}}).dump(), "application/json");
        }
    });

    // ==========================================
    // API: Get Internships
    // ==========================================
    svr.Get("/api/internships", [](const httplib::Request& req, httplib::Response& res) {
        json arr = json::array();
        for(auto& i : internships) {
            i.autoClose();
            if(i.isOpen()) {
                Company* c = findCompany(i.getCompanyID());
                arr.push_back({
                    {"id", i.getId()},
                    {"role", i.getRole()},
                    {"companyName", c ? c->getName() : "Unknown"},
                    {"stipend", i.getStipend()},
                    {"duration", i.getDuration()},
                    {"location", i.getLocation()},
                    {"deadline", i.getDeadline()},
                    {"minCGPA", i.getMinCGPA()},
                    {"skills", i.getRequiredSkills()}
                });
            }
        }
        res.set_content(json({{"success", true}, {"data", arr}}).dump(), "application/json");
    });

    // ==========================================
    // API: Apply to Internship
    // ==========================================
    svr.Post("/api/apply", [](const httplib::Request& req, httplib::Response& res) {
        try {
            string authHeader = req.get_header_value("Authorization");
            if(authHeader.empty()) { res.set_content(json({{"success", false}, {"message", "Unauthorized"}}).dump(), "application/json"); return; }
            int studentId = stoi(authHeader.substr(7)); // "Bearer {id}"
            
            auto body = json::parse(req.body);
            int internshipId = body["internshipId"];
            
            Student* stu = findStudent(studentId);
            Internship* intern = findInternship(internshipId);
            if(!stu || !intern || !intern->isOpen()) {
                res.set_content(json({{"success", false}, {"message", "Invalid Application"}}).dump(), "application/json"); return;
            }
            
            for(auto& a : applications) {
                if(a.getStudentID() == studentId && a.getInternshipID() == internshipId) {
                    res.set_content(json({{"success", false}, {"message", "Already applied to this internship"}}).dump(), "application/json"); return;
                }
            }
            
            int score = ResumeScorer::calculateScore(*stu, *intern);
            int matchPct = ResumeScorer::calculateMatchPercentage(*stu, *intern);
            int aid = nextAppID();
            
            // Push
            Application app(aid, studentId, internshipId, DateUtil::getCurrentDate(), score, matchPct);
            applications.push_back(app);
            intern->incrementApplicants();
            if(intern->getCurrentApplicants() >= intern->getMaxApplicants()) intern->setStatus("CLOSED");
            stu->addApplication(aid);
            
            saveAll();
            res.set_content(json({{"success", true}}).dump(), "application/json");
        } catch(...) {
            res.set_content(json({{"success", false}, {"message", "Application Failed"}}).dump(), "application/json");
        }
    });

    // ==========================================
    // API: Student Dashboard
    // ==========================================
    svr.Get("/api/dashboard/student", [](const httplib::Request& req, httplib::Response& res) {
        try {
            string authHeader = req.get_header_value("Authorization");
            if(authHeader.empty()) { res.set_content(json({{"success", false}, {"message", "Unauthorized"}}).dump(), "application/json"); return; }
            int studentId = stoi(authHeader.substr(7)); // "Bearer {id}"
            
            Student* stu = findStudent(studentId);
            if(!stu) { res.set_content(json({{"success", false}, {"message", "Student Not Found"}}).dump(), "application/json"); return; }
            
            json dash;
            dash["totalApps"] = 0; dash["applied"] = 0; dash["shortlisted"] = 0; dash["selected"] = 0;
            json appsArr = json::array();
            
            for(auto& a : applications) {
                if(a.getStudentID() == studentId) {
                    dash["totalApps"] = dash["totalApps"].get<int>() + 1;
                    if(a.getStatus() == "APPLIED") dash["applied"] = dash["applied"].get<int>() + 1;
                    else if(a.getStatus() == "SHORTLISTED") dash["shortlisted"] = dash["shortlisted"].get<int>() + 1;
                    else if(a.getStatus() == "SELECTED") dash["selected"] = dash["selected"].get<int>() + 1;
                    
                    Internship* intern = findInternship(a.getInternshipID());
                    Company* comp = intern ? findCompany(intern->getCompanyID()) : nullptr;
                    appsArr.push_back({
                        {"id", a.getId()},
                        {"role", intern ? intern->getRole() : "Unknown"},
                        {"companyName", comp ? comp->getName() : "Unknown"},
                        {"status", a.getStatus()},
                        {"date", a.getDate()},
                        {"score", a.getResumeScore()},
                        {"match", a.getMatchPercentage()}
                    });
                }
            }
            dash["applications"] = appsArr;
            res.set_content(json({{"success", true}, {"data", dash}}).dump(), "application/json");
        } catch(...) {
            res.set_content(json({{"success", false}, {"message", "Load Failed"}}).dump(), "application/json");
        }
    });

    std::cout << "Server Listening on Port 8080. Go to http://localhost:8080 in a web browser.\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}
