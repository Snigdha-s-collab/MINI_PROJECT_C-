#include "Engine.h"

class SIAMS {
private:
    vector<Student> students;
    vector<Company> companies;
    vector<Internship> internships;
    vector<Application> applications;
    vector<Notification> notifications;
    FileManager fm;
    int currentUserID;
    string currentUserType;

    int nextStudentID() { int m=0; for(auto&s:students) m=max(m,s.getId()); return m+1; }
    int nextCompanyID() { int m=0; for(auto&c:companies) m=max(m,c.getId()); return m+1; }
    int nextInternshipID() { int m=0; for(auto&i:internships) m=max(m,i.getId()); return m+1; }
    int nextAppID() { int m=0; for(auto&a:applications) m=max(m,a.getId()); return m+1; }
    int nextNotifID() { int m=0; for(auto&n:notifications) m=max(m,n.getId()); return m+1; }

    Student* findStudent(int id) { for(auto&s:students) if(s.getId()==id) return &s; return nullptr; }
    Company* findCompany(int id) { for(auto&c:companies) if(c.getId()==id) return &c; return nullptr; }
    Internship* findInternship(int id) { for(auto&i:internships) if(i.getId()==id) return &i; return nullptr; }
    Application* findApplication(int id) { for(auto&a:applications) if(a.getId()==id) return &a; return nullptr; }

    void addNotif(int uid, const string& utype, const string& msg) {
        notifications.push_back(Notification(nextNotifID(), uid, utype, msg));
    }

    void saveAll() { fm.saveAll(students, companies, internships, applications, notifications); }

public:
    SIAMS() : currentUserID(0) {
        students = fm.loadStudents();
        companies = fm.loadCompanies();
        internships = fm.loadInternships();
        applications = fm.loadApplications();
        notifications = fm.loadNotifications();
        for(auto&i:internships) i.autoClose();
    }

    // ============ CHANGE PASSWORD ============
    void changePassword() {
        ConsoleUI::printHeader("CHANGE PASSWORD");
        string oldPwd = ConsoleUI::getInput("Current Password");
        User* user = nullptr;
        if(currentUserType=="student") user = findStudent(currentUserID);
        else user = findCompany(currentUserID);
        if(!user || !user->checkPassword(oldPwd)) { ConsoleUI::printError("Wrong current password!"); ConsoleUI::pressEnter(); return; }
        string newPwd = ConsoleUI::getInput("New Password");
        string confirm = ConsoleUI::getInput("Confirm New Password");
        if(newPwd != confirm) { ConsoleUI::printError("Passwords don't match!"); ConsoleUI::pressEnter(); return; }
        if(newPwd.size() < 4) { ConsoleUI::printError("Password too short (min 4 chars)!"); ConsoleUI::pressEnter(); return; }
        user->changePassword(newPwd);
        saveAll();
        ConsoleUI::printSuccess("Password changed successfully!");
        ConsoleUI::pressEnter();
    }

    // ============ EXPORT REPORT ============
    void exportStudentReport() {
        Student* stu = findStudent(currentUserID);
        if(!stu) return;
        string filename = "report_student_S" + to_string(currentUserID) + ".txt";
        ofstream f(filename);
        f << stu->exportReport() << "\n";
        f << "\n=== Application History ===\n";
        for(auto&a:applications) {
            if(a.getStudentID()!=currentUserID) continue;
            Internship* intern = findInternship(a.getInternshipID());
            Company* comp = intern ? findCompany(intern->getCompanyID()) : nullptr;
            f << "App A" << a.getId() << " | Role: " << (intern?intern->getRole():"?") << " | Company: " << (comp?comp->getName():"?") << " | Score: " << a.getResumeScore() << " | Match: " << a.getMatchPercentage() << "% | Status: " << a.getStatus() << " | Date: " << a.getDate();
            if(!a.getRemarks().empty()) f << " | Remarks: " << a.getRemarks();
            f << "\n";
        }
        f.close();
        ConsoleUI::printSuccess("Report exported to: " + filename);
        ConsoleUI::pressEnter();
    }

    void exportCompanyReport() {
        Company* comp = findCompany(currentUserID);
        if(!comp) return;
        string filename = "report_company_C" + to_string(currentUserID) + ".txt";
        ofstream f(filename);
        f << comp->exportReport() << "\n";
        f << "\n=== Posted Internships ===\n";
        for(auto&i:internships) {
            if(i.getCompanyID()!=currentUserID) continue;
            f << "I" << i.getId() << " | " << i.getRole() << " | Stipend: Rs." << i.getStipend() << " | Location: " << i.getLocation() << " | Applicants: " << i.getCurrentApplicants() << "/" << i.getMaxApplicants() << " | Status: " << i.getStatus() << "\n";
        }
        f << "\n=== Applications Received ===\n";
        for(auto&a:applications) {
            Internship* intern = findInternship(a.getInternshipID());
            if(!intern || intern->getCompanyID()!=currentUserID) continue;
            Student* s = findStudent(a.getStudentID());
            f << "A" << a.getId() << " | Student: " << (s?s->getName():"?") << " | Role: " << intern->getRole() << " | Score: " << a.getResumeScore() << " | Status: " << a.getStatus() << "\n";
        }
        f.close();
        ConsoleUI::printSuccess("Report exported to: " + filename);
        ConsoleUI::pressEnter();
    }

    // ============ SEARCH INTERNSHIPS BY KEYWORD ============
    void searchInternships() {
        ConsoleUI::printHeader("SEARCH INTERNSHIPS");
        string keyword = ConsoleUI::getInput("Enter keyword (role/company/skill)");
        if(keyword.empty()) return;
        Student* stu = findStudent(currentUserID);
        vector<Internship> results;
        for(auto&i:internships) {
            i.autoClose();
            if(!i.isOpen()) continue;
            Company* c = findCompany(i.getCompanyID());
            bool match = StrUtil::containsIgnoreCase(i.getRole(), keyword) || StrUtil::containsIgnoreCase(i.getLocation(), keyword);
            if(c && StrUtil::containsIgnoreCase(c->getName(), keyword)) match = true;
            for(auto&sk:i.getRequiredSkills()) if(StrUtil::containsIgnoreCase(sk, keyword)) { match=true; break; }
            if(match) results.push_back(i);
        }
        if(results.empty()) { ConsoleUI::printInfo("No results for '" + keyword + "'."); ConsoleUI::pressEnter(); return; }
        ConsoleUI::printSubHeader("Found " + to_string(results.size()) + " result(s) for '" + keyword + "'");
        for(size_t idx=0; idx<results.size(); idx++) {
            auto& intern = results[idx];
            Company* c = findCompany(intern.getCompanyID());
            cout << Color::BOLD << Color::BMAGENTA << "  [" << (idx+1) << "] ";
            if(c) { cout << c->getName() << " "; ConsoleUI::printStars(c->getRating()); }
            cout << Color::RESET << "\n" << intern;
            if(stu) {
                int mp = ResumeScorer::calculateMatchPercentage(*stu, intern);
                cout << Color::CYAN << "    Your Match: "; ConsoleUI::printProgressBar(mp, 100, 15); cout << "\n";
            }
            ConsoleUI::printDivider();
        }
        ConsoleUI::pressEnter();
    }

    // ============ VIEW APPLICATION DETAILS ============
    void viewApplicationDetails() {
        ConsoleUI::printHeader("APPLICATION DETAILS");
        int aid = ConsoleUI::getInt("Enter Application ID");
        Application* app = findApplication(aid);
        if(!app) { ConsoleUI::printError("Application not found!"); ConsoleUI::pressEnter(); return; }
        // Verify ownership
        if(currentUserType=="student" && app->getStudentID()!=currentUserID) { ConsoleUI::printError("Not your application!"); ConsoleUI::pressEnter(); return; }
        Internship* intern = findInternship(app->getInternshipID());
        Company* comp = intern ? findCompany(intern->getCompanyID()) : nullptr;
        Student* stu = findStudent(app->getStudentID());
        if(currentUserType=="company" && (!intern || intern->getCompanyID()!=currentUserID)) { ConsoleUI::printError("Not your internship!"); ConsoleUI::pressEnter(); return; }
        ConsoleUI::printSubHeader("Application A" + to_string(aid));
        cout << Color::CYAN << "  Student     : " << Color::WHITE << (stu?stu->getName():"?") << " (S" << app->getStudentID() << ")\n";
        cout << Color::CYAN << "  Role        : " << Color::WHITE << (intern?intern->getRole():"?") << "\n";
        cout << Color::CYAN << "  Company     : " << Color::WHITE << (comp?comp->getName():"?") << "\n";
        cout << Color::CYAN << "  Applied On  : " << Color::WHITE << app->getDate() << "\n";
        cout << Color::CYAN << "  Resume Score: " << Color::WHITE << app->getResumeScore() << "/100\n";
        cout << Color::CYAN << "  Skill Match : "; ConsoleUI::printProgressBar(app->getMatchPercentage(), 100); cout << "\n";
        cout << Color::CYAN << "  Status      : " << app->getStatusColor() << app->getStatus() << Color::RESET << "\n";
        if(!app->getRemarks().empty()) cout << Color::CYAN << "  Remarks     : " << Color::WHITE << app->getRemarks() << "\n";
        if(stu && intern) {
            auto missing = ResumeScorer::getMissingSkills(*stu, *intern);
            if(!missing.empty()) ConsoleUI::printInfo("Missing skills: " + StrUtil::join(missing, ", "));
        }
        cout << Color::RESET;
        ConsoleUI::pressEnter();
    }

    // ============ REGISTRATION ============
    void registerStudent() {
        ConsoleUI::printHeader("STUDENT REGISTRATION");
        string name = ConsoleUI::getInput("Full Name");
        string email = ConsoleUI::getInput("Email");
        for(auto&s:students) { if(s.getEmail()==email) { ConsoleUI::printError("Email already registered!"); return; } }
        string pwd = ConsoleUI::getInput("Password");
        float cgpa = ConsoleUI::getFloat("CGPA (0.0-10.0)", 0, 10);
        string sk = ConsoleUI::getInput("Skills (comma-separated)");
        vector<string> skills = StrUtil::split(sk, ',');
        string resume = ConsoleUI::getInput("Brief Resume/About");
        Student s(nextStudentID(), name, email, StrUtil::hashPassword(pwd), cgpa, skills, resume);
        students.push_back(s);
        saveAll();
        ConsoleUI::printSuccess("Registered! Your ID: S" + to_string(s.getId()));
        ConsoleUI::pressEnter();
    }

    void registerCompany() {
        ConsoleUI::printHeader("COMPANY REGISTRATION");
        string name = ConsoleUI::getInput("Company Name");
        string email = ConsoleUI::getInput("Email");
        for(auto&c:companies) { if(c.getEmail()==email) { ConsoleUI::printError("Email already registered!"); return; } }
        string pwd = ConsoleUI::getInput("Password");
        string industry = ConsoleUI::getInput("Industry (IT/Finance/Consulting/Manufacturing/Other)");
        Company c(nextCompanyID(), name, email, StrUtil::hashPassword(pwd), industry);
        companies.push_back(c);
        saveAll();
        ConsoleUI::printSuccess("Registered! Your ID: C" + to_string(c.getId()));
        ConsoleUI::pressEnter();
    }

    // ============ LOGIN ============
    int loginStudent() {
        ConsoleUI::printHeader("STUDENT LOGIN");
        string email = ConsoleUI::getInput("Email");
        string pwd = ConsoleUI::getInput("Password");
        for(auto&s:students) {
            if(s.getEmail()==email && s.checkPassword(pwd)) {
                currentUserID = s.getId(); currentUserType = "student";
                ConsoleUI::printSuccess("Welcome back, " + s.getName() + "!");
                return s.getId();
            }
        }
        ConsoleUI::printError("Invalid credentials!"); ConsoleUI::pressEnter(); return -1;
    }

    int loginCompany() {
        ConsoleUI::printHeader("COMPANY LOGIN");
        string email = ConsoleUI::getInput("Email");
        string pwd = ConsoleUI::getInput("Password");
        for(auto&c:companies) {
            if(c.getEmail()==email && c.checkPassword(pwd)) {
                currentUserID = c.getId(); currentUserType = "company";
                ConsoleUI::printSuccess("Welcome back, " + c.getName() + "!");
                return c.getId();
            }
        }
        ConsoleUI::printError("Invalid credentials!"); ConsoleUI::pressEnter(); return -1;
    }

    // ============ POST INTERNSHIP (Company) ============
    void postInternship() {
        ConsoleUI::printHeader("POST NEW INTERNSHIP");
        Company* comp = findCompany(currentUserID);
        if(!comp) return;
        string role = ConsoleUI::getInput("Role/Title");
        string sk = ConsoleUI::getInput("Required Skills (comma-separated)");
        vector<string> skills = StrUtil::split(sk, ',');
        float minCGPA = ConsoleUI::getFloat("Minimum CGPA required", 0, 10);
        int stipend = ConsoleUI::getInt("Stipend (Rs/month)", 0, 999999);
        int duration = ConsoleUI::getInt("Duration (months)", 1, 24);
        string location = ConsoleUI::getInput("Location");
        string deadline;
        while(true) {
            deadline = ConsoleUI::getInput("Application Deadline (DD-MM-YYYY)");
            if(DateUtil::isValidDate(deadline) && !DateUtil::isDeadlinePassed(deadline)) break;
            ConsoleUI::printError("Invalid or past date. Use DD-MM-YYYY format.");
        }
        int maxApp = ConsoleUI::getInt("Max Applicants", 1, 10000);
        int iid = nextInternshipID();
        Internship intern(iid, currentUserID, role, skills, minCGPA, stipend, duration, location, deadline, maxApp);
        internships.push_back(intern);
        comp->addInternship(iid);
        saveAll();
        ConsoleUI::printSuccess("Internship posted! ID: I" + to_string(iid));
        // Notify matching students
        for(auto&s:students) {
            int match = ResumeScorer::calculateMatchPercentage(s, intern);
            if(match >= 40) addNotif(s.getId(), "student", "New internship matching your skills: " + role + " at " + comp->getName());
        }
        saveAll();
        ConsoleUI::pressEnter();
    }

    // ============ BROWSE INTERNSHIPS (Student) ============
    void browseInternships() {
        ConsoleUI::printHeader("BROWSE INTERNSHIPS");
        cout << Color::CYAN << "  Filter options:\n";
        ConsoleUI::printMenuItem(1, "View All Open");
        ConsoleUI::printMenuItem(2, "Filter by Min Stipend");
        ConsoleUI::printMenuItem(3, "Filter by Location");
        ConsoleUI::printMenuItem(4, "Filter by Skills");
        ConsoleUI::printMenuItem(5, "Sort by Stipend (High to Low)");
        ConsoleUI::printMenuItem(6, "Sort by Deadline (Nearest)");
        ConsoleUI::printMenuItem(7, "Sort by Match % (Best First)");
        int ch = ConsoleUI::getChoice(1, 7);

        vector<Internship> filtered;
        for(auto&i:internships) { i.autoClose(); if(i.isOpen()) filtered.push_back(i); }

        if(ch==2) {
            int minS = ConsoleUI::getInt("Minimum stipend", 0, 999999);
            filtered = filterItems<Internship>(filtered, [minS](const Internship&i){ return i.getStipend()>=minS; });
        } else if(ch==3) {
            string loc = ConsoleUI::getInput("Location");
            filtered = filterItems<Internship>(filtered, [loc](const Internship&i){ return StrUtil::toLower(i.getLocation()).find(StrUtil::toLower(loc))!=string::npos; });
        } else if(ch==4) {
            string sk = ConsoleUI::getInput("Skill to search");
            filtered = filterItems<Internship>(filtered, [sk](const Internship&i){
                for(auto&s:i.getRequiredSkills()) if(StrUtil::toLower(s).find(StrUtil::toLower(sk))!=string::npos) return true;
                return false;
            });
        }

        Student* stu = findStudent(currentUserID);
        if(ch==5) sort(filtered.begin(), filtered.end(), [](auto&a,auto&b){ return a.getStipend()>b.getStipend(); });
        else if(ch==6) sort(filtered.begin(), filtered.end(), [](auto&a,auto&b){ return DateUtil::daysRemaining(a.getDeadline())<DateUtil::daysRemaining(b.getDeadline()); });
        else if(ch==7 && stu) sort(filtered.begin(), filtered.end(), [&](auto&a,auto&b){ return ResumeScorer::calculateMatchPercentage(*stu,a)>ResumeScorer::calculateMatchPercentage(*stu,b); });

        if(filtered.empty()) { ConsoleUI::printInfo("No internships found."); ConsoleUI::pressEnter(); return; }

        ConsoleUI::printSubHeader("Found " + to_string(filtered.size()) + " Internship(s)");
        for(size_t idx=0; idx<filtered.size(); idx++) {
            auto& intern = filtered[idx];
            Company* c = findCompany(intern.getCompanyID());
            cout << Color::BOLD << Color::BMAGENTA << "  [" << (idx+1) << "] ";
            if(c) { cout << c->getName(); ConsoleUI::printStars(c->getRating()); }
            cout << Color::RESET << "\n";
            cout << intern;
            if(stu) {
                int mp = ResumeScorer::calculateMatchPercentage(*stu, intern);
                cout << Color::CYAN << "    Your Match: "; ConsoleUI::printProgressBar(mp, 100, 15); cout << "\n";
            }
            ConsoleUI::printDivider();
        }
        ConsoleUI::pressEnter();
    }

    // ============ APPLY TO INTERNSHIP ============
    void applyToInternship() {
        ConsoleUI::printHeader("APPLY TO INTERNSHIP");
        Student* stu = findStudent(currentUserID);
        if(!stu) return;
        int iid = ConsoleUI::getInt("Enter Internship ID");
        Internship* intern = findInternship(iid);
        if(!intern) { ConsoleUI::printError("Internship not found!"); ConsoleUI::pressEnter(); return; }
        if(!intern->isOpen()) { ConsoleUI::printError("This internship is closed!"); ConsoleUI::pressEnter(); return; }
        // Conflict detection: duplicate
        for(auto&a:applications) {
            if(a.getStudentID()==currentUserID && a.getInternshipID()==iid) {
                ConsoleUI::printError("You already applied to this internship!"); ConsoleUI::pressEnter(); return;
            }
        }
        // Conflict: already selected elsewhere
        for(auto&a:applications) {
            if(a.getStudentID()==currentUserID && a.getStatus()=="SELECTED") {
                ConsoleUI::printWarning("Note: You are already SELECTED for another internship (App ID: " + to_string(a.getId()) + ")");
            }
        }
        // CGPA warning
        if(stu->getCgpa() < intern->getMinCGPA()) {
            ConsoleUI::printWarning("Your CGPA (" + to_string(stu->getCgpa()).substr(0,3) + ") is below minimum (" + to_string(intern->getMinCGPA()).substr(0,3) + "). Apply anyway?");
            cout << Color::CYAN << "  Continue? (y/n): " << Color::WHITE;
            string yn; getline(cin, yn);
            if(yn != "y" && yn != "Y") return;
        }
        int score = ResumeScorer::calculateScore(*stu, *intern);
        int matchPct = ResumeScorer::calculateMatchPercentage(*stu, *intern);
        int aid = nextAppID();
        Application app(aid, currentUserID, iid, DateUtil::getCurrentDate(), score, matchPct);
        applications.push_back(app);
        intern->incrementApplicants();
        if(intern->getCurrentApplicants() >= intern->getMaxApplicants()) intern->setStatus("CLOSED");
        stu->addApplication(aid);
        addNotif(intern->getCompanyID(), "company", "New application for " + intern->getRole() + " from " + stu->getName());
        // Show missing skills
        auto missing = ResumeScorer::getMissingSkills(*stu, *intern);
        ConsoleUI::printSuccess("Application submitted! ID: A" + to_string(aid));
        cout << Color::CYAN << "  Resume Score: " << Color::WHITE << score << "/100\n";
        cout << Color::CYAN << "  Skill Match:  "; ConsoleUI::printProgressBar(matchPct, 100); cout << "\n";
        if(!missing.empty()) {
            ConsoleUI::printInfo("Skills to improve: " + StrUtil::join(missing, ", "));
        }
        saveAll();
        ConsoleUI::pressEnter();
    }

    // ============ VIEW MY APPLICATIONS (Student) ============
    void viewMyApplications() {
        ConsoleUI::printHeader("MY APPLICATIONS");
        Student* stu = findStudent(currentUserID);
        if(!stu) return;
        vector<string> hdr = {"AppID", "Role", "Company", "Score", "Match%", "Status", "Date"};
        vector<int> widths = {8, 18, 15, 7, 8, 13, 12};
        ConsoleUI::printTableHeader(hdr, widths);
        int count = 0;
        for(auto&a:applications) {
            if(a.getStudentID() != currentUserID) continue;
            Internship* intern = findInternship(a.getInternshipID());
            Company* comp = intern ? findCompany(intern->getCompanyID()) : nullptr;
            vector<string> row = {
                "A"+to_string(a.getId()),
                intern ? intern->getRole() : "?",
                comp ? comp->getName() : "?",
                to_string(a.getResumeScore()),
                to_string(a.getMatchPercentage())+"%",
                a.getStatusColor() + a.getStatus() + Color::RESET,
                a.getDate()
            };
            ConsoleUI::printTableRow(row, widths);
            count++;
        }
        if(count==0) ConsoleUI::printInfo("No applications yet.");
        ConsoleUI::pressEnter();
    }

    // ============ COMPANY: VIEW APPLICANTS ============
    void viewApplicants() {
        ConsoleUI::printHeader("VIEW APPLICANTS");
        Company* comp = findCompany(currentUserID);
        if(!comp) return;
        // List company's internships
        vector<Internship*> myInterns;
        for(auto&i:internships) if(i.getCompanyID()==currentUserID) myInterns.push_back(&i);
        if(myInterns.empty()) { ConsoleUI::printInfo("No internships posted yet."); ConsoleUI::pressEnter(); return; }
        cout << Color::CYAN << "  Your Internships:\n";
        for(size_t i=0;i<myInterns.size();i++) {
            cout << Color::BYELLOW << "    [" << (i+1) << "] " << Color::WHITE << myInterns[i]->getRole()
                 << Color::DIM << " (I" << myInterns[i]->getId() << ", " << myInterns[i]->getCurrentApplicants() << " applicants)\n";
        }
        int sel = ConsoleUI::getInt("Select internship #", 1, (int)myInterns.size()) - 1;
        int iid = myInterns[sel]->getId();

        ConsoleUI::printSubHeader("Applicants for: " + myInterns[sel]->getRole());
        ConsoleUI::printMenuItem(1, "View All");
        ConsoleUI::printMenuItem(2, "Auto-Shortlist (Top 30%)");
        ConsoleUI::printMenuItem(3, "Filter by Min CGPA");
        ConsoleUI::printMenuItem(4, "Change Application Status");
        ConsoleUI::printMenuItem(5, "Bulk Reject Below Score");
        int action = ConsoleUI::getChoice(1, 5);

        if(action==2) {
            auto shortlisted = MatchEngine::autoShortlist(applications, iid);
            ConsoleUI::printSuccess("Auto-shortlisted " + to_string(shortlisted.size()) + " applicant(s)!");
            for(int sid : shortlisted) {
                addNotif(sid, "student", "Your application for " + myInterns[sel]->getRole() + " has been SHORTLISTED!");
            }
            saveAll();
        }

        if(action==4) {
            int aid = ConsoleUI::getInt("Application ID to update");
            Application* app = findApplication(aid);
            if(!app || app->getInternshipID()!=iid) { ConsoleUI::printError("Invalid App ID"); ConsoleUI::pressEnter(); return; }
            cout << Color::CYAN << "  New Status:\n";
            ConsoleUI::printMenuItem(1, "SHORTLISTED"); ConsoleUI::printMenuItem(2, "REJECTED");
            ConsoleUI::printMenuItem(3, "SELECTED");
            int st = ConsoleUI::getChoice(1, 3);
            string statuses[] = {"SHORTLISTED", "REJECTED", "SELECTED"};
            app->setStatus(statuses[st-1]);
            string remarks = ConsoleUI::getInput("Remarks (optional)");
            if(!remarks.empty()) app->setRemarks(remarks);
            if(st==3) comp->incrementHires();
            addNotif(app->getStudentID(), "student", "Application for " + myInterns[sel]->getRole() + ": " + statuses[st-1]);
            saveAll();
            ConsoleUI::printSuccess("Status updated!");
            ConsoleUI::pressEnter(); return;
        }

        if(action==5) {
            int minScore = ConsoleUI::getInt("Reject all below score", 0, 100);
            int cnt=0;
            for(auto&a:applications) {
                if(a.getInternshipID()==iid && a.getStatus()=="APPLIED" && a.getResumeScore()<minScore) {
                    a.setStatus("REJECTED"); cnt++;
                    addNotif(a.getStudentID(), "student", "Application for " + myInterns[sel]->getRole() + ": REJECTED");
                }
            }
            ConsoleUI::printSuccess("Rejected " + to_string(cnt) + " applicant(s).");
            saveAll(); ConsoleUI::pressEnter(); return;
        }

        // Display applicants table
        float minCGPA = 0;
        if(action==3) minCGPA = ConsoleUI::getFloat("Minimum CGPA", 0, 10);
        vector<string> hdr = {"AppID", "Student", "CGPA", "Score", "Match%", "Status"};
        vector<int> widths = {8, 20, 7, 7, 8, 13};
        ConsoleUI::printTableHeader(hdr, widths);
        for(auto&a:applications) {
            if(a.getInternshipID()!=iid) continue;
            Student* s = findStudent(a.getStudentID());
            if(!s) continue;
            if(action==3 && s->getCgpa()<minCGPA) continue;
            vector<string> row = {"A"+to_string(a.getId()), s->getName(),
                to_string(s->getCgpa()).substr(0,3), to_string(a.getResumeScore()),
                to_string(a.getMatchPercentage())+"%", a.getStatusColor()+a.getStatus()+Color::RESET};
            ConsoleUI::printTableRow(row, widths);
        }
        ConsoleUI::pressEnter();
    }

    // ============ STUDENT DASHBOARD ============
    void studentDashboard() {
        ConsoleUI::printHeader("STUDENT DASHBOARD");
        Student* stu = findStudent(currentUserID);
        if(!stu) return;
        stu->displayDashboardHeader();
        ConsoleUI::printDivider();
        int total=0,applied=0,shortlisted=0,rejected=0,selected=0;
        int totalMatch=0;
        for(auto&a:applications) {
            if(a.getStudentID()!=currentUserID) continue;
            total++; totalMatch+=a.getMatchPercentage();
            if(a.getStatus()=="APPLIED") applied++;
            else if(a.getStatus()=="SHORTLISTED") shortlisted++;
            else if(a.getStatus()=="REJECTED") rejected++;
            else if(a.getStatus()=="SELECTED") selected++;
        }
        cout << Color::CYAN << "  Total Applications : " << Color::BOLD << Color::WHITE << total << "\n";
        cout << Color::BYELLOW << "    Applied     : " << applied << "  ";
        cout << Color::BCYAN << "  Shortlisted : " << shortlisted << "  ";
        cout << Color::BRED << "  Rejected    : " << rejected << "  ";
        cout << Color::BGREEN << "  Selected    : " << selected << "\n" << Color::RESET;
        if(total>0) {
            float successRate = ((float)(shortlisted+selected)/total)*100;
            cout << Color::CYAN << "  Avg Match %  : " << Color::WHITE << (totalMatch/total) << "%\n";
            cout << Color::CYAN << "  Success Rate : "; ConsoleUI::printProgressBar((int)successRate, 100); cout << "\n";
        }
        // Skill improvement suggestions
        map<string,int> missingCount;
        for(auto&a:applications) {
            if(a.getStudentID()!=currentUserID) continue;
            Internship* intern = findInternship(a.getInternshipID());
            if(!intern) continue;
            auto missing = ResumeScorer::getMissingSkills(*stu, *intern);
            for(auto&m:missing) missingCount[StrUtil::toLower(m)]++;
        }
        if(!missingCount.empty()) {
            vector<pair<int,string>> sorted;
            for(auto&p:missingCount) sorted.push_back({p.second, p.first});
            sort(sorted.rbegin(), sorted.rend());
            cout << Color::BYELLOW << "\n  Recommended Skills to Learn:\n" << Color::RESET;
            for(size_t i=0;i<min((size_t)5,sorted.size());i++) {
                cout << Color::WHITE << "    -> " << Color::BGREEN << sorted[i].second << Color::DIM << " (needed in " << sorted[i].first << " applications)\n";
            }
        }
        cout << Color::RESET;
        ConsoleUI::pressEnter();
    }

    // ============ COMPANY DASHBOARD ============
    void companyDashboard() {
        ConsoleUI::printHeader("COMPANY DASHBOARD");
        Company* comp = findCompany(currentUserID);
        if(!comp) return;
        comp->displayDashboardHeader();
        ConsoleUI::printDivider();
        int totalInterns=0, totalApps=0, totalShortlisted=0, totalSelected=0;
        float totalScore=0;
        map<string,int> skillDemand;
        for(auto&i:internships) {
            if(i.getCompanyID()!=currentUserID) continue;
            totalInterns++;
        }
        for(auto&a:applications) {
            Internship* intern = findInternship(a.getInternshipID());
            if(!intern || intern->getCompanyID()!=currentUserID) continue;
            totalApps++; totalScore+=a.getResumeScore();
            if(a.getStatus()=="SHORTLISTED") totalShortlisted++;
            if(a.getStatus()=="SELECTED") totalSelected++;
            Student* s = findStudent(a.getStudentID());
            if(s) for(auto&sk:s->getSkills()) skillDemand[StrUtil::toLower(sk)]++;
        }
        cout << Color::CYAN << "  Internships Posted    : " << Color::WHITE << totalInterns << "\n";
        cout << Color::CYAN << "  Total Applications    : " << Color::WHITE << totalApps << "\n";
        if(totalApps>0) {
            cout << Color::CYAN << "  Avg Applicant Score   : " << Color::WHITE << (int)(totalScore/totalApps) << "/100\n";
            float convRate = (totalShortlisted>0) ? ((float)totalSelected/totalShortlisted)*100 : 0;
            cout << Color::CYAN << "  Shortlist->Hire Rate  : "; ConsoleUI::printProgressBar((int)convRate, 100); cout << "\n";
        }
        if(!skillDemand.empty()) {
            vector<pair<int,string>> sorted;
            for(auto&p:skillDemand) sorted.push_back({p.second, p.first});
            sort(sorted.rbegin(), sorted.rend());
            cout << Color::BYELLOW << "\n  Most Common Applicant Skills:\n" << Color::RESET;
            for(size_t i=0;i<min((size_t)5,sorted.size());i++) {
                cout << Color::WHITE << "    " << (i+1) << ". " << Color::BGREEN << sorted[i].second << Color::DIM << " (" << sorted[i].first << " applicants)\n";
            }
        }
        cout << Color::RESET;
        ConsoleUI::pressEnter();
    }

    // ============ NOTIFICATIONS ============
    void viewNotifications() {
        ConsoleUI::printHeader("NOTIFICATIONS");
        int unread = 0;
        vector<Notification*> mine;
        for(auto&n:notifications) {
            if(n.getUserID()==currentUserID && n.getUserType()==currentUserType) {
                mine.push_back(&n);
                if(!n.getIsRead()) unread++;
            }
        }
        cout << Color::BCYAN << "  Unread: " << Color::BOLD << unread << Color::RESET << "\n\n";
        if(mine.empty()) { ConsoleUI::printInfo("No notifications."); ConsoleUI::pressEnter(); return; }
        for(int i=(int)mine.size()-1; i>=0; i--) {
            string icon = mine[i]->getIsRead() ? "  " : Color::BGREEN + "* " + Color::RESET;
            cout << icon << Color::DIM << "[" << mine[i]->getTimestamp() << "] " << Color::RESET
                 << Color::WHITE << mine[i]->getMessage() << "\n";
        }
        // Mark all as read
        for(auto*n:mine) n->markRead();
        saveAll();
        ConsoleUI::pressEnter();
    }

    // ============ RECOMMENDATIONS ============
    void viewRecommendations() {
        ConsoleUI::printHeader("RECOMMENDED INTERNSHIPS");
        Student* stu = findStudent(currentUserID);
        if(!stu) return;
        auto recs = MatchEngine::recommendForStudent(*stu, internships, applications);
        if(recs.empty()) { ConsoleUI::printInfo("No recommendations at this time."); ConsoleUI::pressEnter(); return; }
        int shown = 0;
        for(auto&[score, intern] : recs) {
            if(shown >= 5) break;
            Company* c = findCompany(intern.getCompanyID());
            cout << Color::BMAGENTA << "  Compatibility: " << score << "% " << Color::RESET;
            if(c) cout << Color::DIM << "@ " << c->getName();
            cout << "\n" << intern;
            ConsoleUI::printDivider();
            shown++;
        }
        ConsoleUI::pressEnter();
    }

    void viewCompanyRecommendations() {
        ConsoleUI::printHeader("RECOMMENDED CANDIDATES");
        Company* comp = findCompany(currentUserID);
        if(!comp) return;
        vector<Internship*> myInterns;
        for(auto&i:internships) if(i.getCompanyID()==currentUserID && i.isOpen()) myInterns.push_back(&i);
        if(myInterns.empty()) { ConsoleUI::printInfo("No open internships."); ConsoleUI::pressEnter(); return; }
        for(size_t i=0;i<myInterns.size();i++)
            cout << Color::BYELLOW << "  [" << (i+1) << "] " << Color::WHITE << myInterns[i]->getRole() << "\n";
        int sel = ConsoleUI::getInt("Select internship #", 1, (int)myInterns.size()) - 1;
        auto recs = MatchEngine::recommendForInternship(*myInterns[sel], students, applications);
        if(recs.empty()) { ConsoleUI::printInfo("No candidates to recommend."); ConsoleUI::pressEnter(); return; }
        ConsoleUI::printSubHeader("Top Candidates (haven't applied yet)");
        int shown=0;
        for(auto&[score, stu] : recs) {
            if(shown>=10) break;
            cout << Color::BGREEN << "  Score: " << score << " " << Color::WHITE << stu.getName()
                 << Color::DIM << " (CGPA: " << fixed << setprecision(1) << stu.getCgpa()
                 << ", Skills: " << StrUtil::join(stu.getSkills(), ", ") << ")\n";
            shown++;
        }
        ConsoleUI::pressEnter();
    }

    // ============ RATE COMPANY (Student) ============
    void rateCompany() {
        ConsoleUI::printHeader("RATE A COMPANY");
        // Show companies student has interacted with
        set<int> compIDs;
        for(auto&a:applications) {
            if(a.getStudentID()!=currentUserID) continue;
            if(a.getStatus()=="SHORTLISTED" || a.getStatus()=="SELECTED" || a.getStatus()=="REJECTED") {
                Internship* intern = findInternship(a.getInternshipID());
                if(intern) compIDs.insert(intern->getCompanyID());
            }
        }
        if(compIDs.empty()) { ConsoleUI::printInfo("No companies to rate (apply first)."); ConsoleUI::pressEnter(); return; }
        for(int cid : compIDs) {
            Company* c = findCompany(cid);
            if(c) { cout << Color::WHITE << "  C" << cid << " - " << c->getName() << " "; ConsoleUI::printStars(c->getRating()); cout << "\n"; }
        }
        int cid = ConsoleUI::getInt("Enter Company ID to rate");
        if(!compIDs.count(cid)) { ConsoleUI::printError("Invalid company."); ConsoleUI::pressEnter(); return; }
        int stars = ConsoleUI::getInt("Rating (1-5 stars)", 1, 5);
        Company* c = findCompany(cid);
        c->addRating(stars);
        saveAll();
        ConsoleUI::printSuccess("Rated " + c->getName() + " with " + to_string(stars) + " stars!");
        ConsoleUI::pressEnter();
    }

    // ============ EDIT PROFILE ============
    void editStudentProfile() {
        ConsoleUI::printHeader("EDIT PROFILE");
        Student* stu = findStudent(currentUserID);
        if(!stu) return;
        stu->displayProfile();
        ConsoleUI::printMenuItem(1, "Update Name");
        ConsoleUI::printMenuItem(2, "Update CGPA");
        ConsoleUI::printMenuItem(3, "Update Skills");
        ConsoleUI::printMenuItem(4, "Update Resume");
        ConsoleUI::printMenuItem(5, "Update Email");
        ConsoleUI::printMenuItem(6, "Back");
        int ch = ConsoleUI::getChoice(1, 6);
        if(ch==1) stu->setName(ConsoleUI::getInput("New Name"));
        else if(ch==2) stu->setCgpa(ConsoleUI::getFloat("New CGPA", 0, 10));
        else if(ch==3) { string sk=ConsoleUI::getInput("New Skills (comma-separated)"); stu->setSkills(StrUtil::split(sk,',')); }
        else if(ch==4) stu->setResumeText(ConsoleUI::getInput("New Resume Text"));
        else if(ch==5) stu->setEmail(ConsoleUI::getInput("New Email"));
        else return;
        saveAll();
        ConsoleUI::printSuccess("Profile updated!");
        ConsoleUI::pressEnter();
    }

    void editCompanyProfile() {
        ConsoleUI::printHeader("EDIT COMPANY PROFILE");
        Company* comp = findCompany(currentUserID);
        if(!comp) return;
        comp->displayProfile();
        ConsoleUI::printMenuItem(1, "Update Company Name");
        ConsoleUI::printMenuItem(2, "Update Industry");
        ConsoleUI::printMenuItem(3, "Update Email");
        ConsoleUI::printMenuItem(4, "Back");
        int ch = ConsoleUI::getChoice(1, 4);
        if(ch==1) comp->setName(ConsoleUI::getInput("New Name"));
        else if(ch==2) comp->setIndustry(ConsoleUI::getInput("New Industry"));
        else if(ch==3) comp->setEmail(ConsoleUI::getInput("New Email"));
        else return;
        saveAll();
        ConsoleUI::printSuccess("Profile updated!");
        ConsoleUI::pressEnter();
    }

    // ============ ADMIN PANEL ============
    void adminPanel() {
        ConsoleUI::printHeader("ADMIN PANEL");
        cout << Color::BRED << "  Password: " << Color::RESET;
        string pwd; getline(cin, pwd);
        if(pwd != "admin123") { ConsoleUI::printError("Wrong password!"); ConsoleUI::pressEnter(); return; }
        while(true) {
            ConsoleUI::printHeader("ADMIN DASHBOARD");
            cout << Color::BGREEN << "  System Stats:\n" << Color::RESET;
            cout << Color::CYAN << "    Students     : " << Color::WHITE << students.size() << "\n";
            cout << Color::CYAN << "    Companies    : " << Color::WHITE << companies.size() << "\n";
            cout << Color::CYAN << "    Internships  : " << Color::WHITE << internships.size() << "\n";
            cout << Color::CYAN << "    Applications : " << Color::WHITE << applications.size() << "\n";
            cout << Color::CYAN << "    Notifications: " << Color::WHITE << notifications.size() << "\n\n";
            ConsoleUI::printMenuItem(1, "List All Students");
            ConsoleUI::printMenuItem(2, "List All Companies");
            ConsoleUI::printMenuItem(3, "List All Internships");
            ConsoleUI::printMenuItem(4, "List All Applications");
            ConsoleUI::printMenuItem(5, "Back");
            int ch = ConsoleUI::getChoice(1, 5);
            if(ch==5) return;
            if(ch==1) { for(auto&s:students) { s.displayProfile(); ConsoleUI::printDivider(); } }
            if(ch==2) { for(auto&c:companies) { c.displayProfile(); ConsoleUI::printDivider(); } }
            if(ch==3) { for(auto&i:internships) { cout << i << "\n"; } }
            if(ch==4) {
                vector<string> hdr={"ID","Student","Internship","Score","Match","Status","Date"};
                vector<int> w={6,8,10,6,6,12,12};
                ConsoleUI::printTableHeader(hdr,w);
                for(auto&a:applications) {
                    vector<string> row={"A"+to_string(a.getId()),"S"+to_string(a.getStudentID()),
                        "I"+to_string(a.getInternshipID()),to_string(a.getResumeScore()),
                        to_string(a.getMatchPercentage())+"%",a.getStatus(),a.getDate()};
                    ConsoleUI::printTableRow(row,w);
                }
            }
            ConsoleUI::pressEnter();
        }
    }

    // ============ STUDENT MENU ============
    void studentMenu() {
        while(true) {
            ConsoleUI::clearScreen();
            ConsoleUI::printHeader("STUDENT PORTAL");
            Student* stu = findStudent(currentUserID);
            if(stu) stu->displayDashboardHeader();
            int unread=0;
            for(auto&n:notifications) if(n.getUserID()==currentUserID && n.getUserType()=="student" && !n.getIsRead()) unread++;
            cout << "\n";
            ConsoleUI::printMenuItem(1, "Browse Internships");
            ConsoleUI::printMenuItem(2, "Search Internships");
            ConsoleUI::printMenuItem(3, "Apply to Internship");
            ConsoleUI::printMenuItem(4, "My Applications");
            ConsoleUI::printMenuItem(5, "View Application Details");
            ConsoleUI::printMenuItem(6, "Dashboard & Analytics");
            ConsoleUI::printMenuItem(7, "Recommendations");
            ConsoleUI::printMenuItem(8, "Notifications" + (unread>0 ? " ("+Color::BRED+to_string(unread)+" new"+Color::RESET+")" : ""));
            ConsoleUI::printMenuItem(9, "Rate a Company");
            ConsoleUI::printMenuItem(10, "Edit Profile");
            ConsoleUI::printMenuItem(11, "Change Password");
            ConsoleUI::printMenuItem(12, "Export Report");
            ConsoleUI::printMenuItem(13, "Logout");
            int ch = ConsoleUI::getChoice(1, 13);
            if(ch==1) browseInternships();
            else if(ch==2) searchInternships();
            else if(ch==3) applyToInternship();
            else if(ch==4) viewMyApplications();
            else if(ch==5) viewApplicationDetails();
            else if(ch==6) studentDashboard();
            else if(ch==7) viewRecommendations();
            else if(ch==8) viewNotifications();
            else if(ch==9) rateCompany();
            else if(ch==10) editStudentProfile();
            else if(ch==11) changePassword();
            else if(ch==12) exportStudentReport();
            else if(ch==13) { currentUserID=0; currentUserType=""; return; }
        }
    }

    // ============ COMPANY MENU ============
    void companyMenu() {
        while(true) {
            ConsoleUI::clearScreen();
            ConsoleUI::printHeader("COMPANY PORTAL");
            Company* comp = findCompany(currentUserID);
            if(comp) comp->displayDashboardHeader();
            int unread=0;
            for(auto&n:notifications) if(n.getUserID()==currentUserID && n.getUserType()=="company" && !n.getIsRead()) unread++;
            cout << "\n";
            ConsoleUI::printMenuItem(1, "Post Internship");
            ConsoleUI::printMenuItem(2, "View Applicants");
            ConsoleUI::printMenuItem(3, "View Application Details");
            ConsoleUI::printMenuItem(4, "Dashboard & Analytics");
            ConsoleUI::printMenuItem(5, "Recommended Candidates");
            ConsoleUI::printMenuItem(6, "Notifications" + (unread>0 ? " ("+Color::BRED+to_string(unread)+" new"+Color::RESET+")" : ""));
            ConsoleUI::printMenuItem(7, "Edit Company Profile");
            ConsoleUI::printMenuItem(8, "Change Password");
            ConsoleUI::printMenuItem(9, "Export Report");
            ConsoleUI::printMenuItem(10, "Logout");
            int ch = ConsoleUI::getChoice(1, 10);
            if(ch==1) postInternship();
            else if(ch==2) viewApplicants();
            else if(ch==3) viewApplicationDetails();
            else if(ch==4) companyDashboard();
            else if(ch==5) viewCompanyRecommendations();
            else if(ch==6) viewNotifications();
            else if(ch==7) editCompanyProfile();
            else if(ch==8) changePassword();
            else if(ch==9) exportCompanyReport();
            else if(ch==10) { currentUserID=0; currentUserType=""; return; }
        }
    }

    // ============ MAIN MENU ============
    void run() {
        ConsoleUI::enableColors();
        while(true) {
            ConsoleUI::clearScreen();
            cout << Color::BOLD << Color::BCYAN << R"(
    ╔═══════════════════════════════════════════════════════╗
    ║                                                       ║
    ║    ____  ___    _    __  __ ____                       ║
    ║   / ___|/ _ \  / \  |  \/  / ___|                     ║
    ║   \___ \ | | |/ _ \ | |\/| \___ \                     ║
    ║    ___) | |_|/ ___ \| |  | |___) |                    ║
    ║   |____/|___/_/   \_\_|  |_|____/                     ║
    ║                                                       ║
    ║   Smart Internship Application Management System      ║
    ║                                                       ║
    ╚═══════════════════════════════════════════════════════╝
)" << Color::RESET << "\n";
            ConsoleUI::printMenuItem(1, "Student Login");
            ConsoleUI::printMenuItem(2, "Company Login");
            ConsoleUI::printMenuItem(3, "Student Registration");
            ConsoleUI::printMenuItem(4, "Company Registration");
            ConsoleUI::printMenuItem(5, "Admin Panel");
            ConsoleUI::printMenuItem(6, "Exit");
            int ch = ConsoleUI::getChoice(1, 6);
            if(ch==1) { if(loginStudent()>0) studentMenu(); }
            else if(ch==2) { if(loginCompany()>0) companyMenu(); }
            else if(ch==3) registerStudent();
            else if(ch==4) registerCompany();
            else if(ch==5) adminPanel();
            else if(ch==6) {
                ConsoleUI::printSuccess("Thank you for using SIAMS! Goodbye.");
                saveAll();
                return;
            }
        }
    }
}; // end class SIAMS

// ============ MAIN ENTRY POINT ============
int main() {
    try {
        SIAMS app;
        app.run();
    } catch(const exception& e) {
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
