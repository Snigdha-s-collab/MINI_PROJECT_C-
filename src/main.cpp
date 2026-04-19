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
        ConsoleUI::printHeader("CHANGE YOUR PASSWORD");
        string oldPwd = ConsoleUI::getInput("Enter your current password");
        User* user = nullptr;
        if(currentUserType=="student") user = findStudent(currentUserID);
        else user = findCompany(currentUserID);
        if(!user || !user->checkPassword(oldPwd)) { ConsoleUI::printError("That password is incorrect! Please try again."); ConsoleUI::pressEnter(); return; }
        string newPwd = ConsoleUI::getInput("Enter your new password (minimum 4 characters)");
        string confirm = ConsoleUI::getInput("Re-enter your new password to confirm");
        if(newPwd != confirm) { ConsoleUI::printError("The passwords you entered don't match! Please try again."); ConsoleUI::pressEnter(); return; }
        if(newPwd.size() < 4) { ConsoleUI::printError("Password is too short! It must be at least 4 characters."); ConsoleUI::pressEnter(); return; }
        user->changePassword(newPwd);
        saveAll();
        ConsoleUI::printSuccess("Your password has been changed successfully!");
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
        string keyword = ConsoleUI::getInput("Type a keyword to search (e.g. role name, company name, skill, or city)");
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
        if(results.empty()) { ConsoleUI::printInfo("No internships found matching '" + keyword + "'. Try a different keyword."); ConsoleUI::pressEnter(); return; }
        ConsoleUI::printSubHeader("Found " + to_string(results.size()) + " internship(s) matching '" + keyword + "'");
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

        // Collect applications visible to the current user
        vector<Application*> myApps;
        if(currentUserType=="student") {
            for(auto&a:applications) if(a.getStudentID()==currentUserID) myApps.push_back(&a);
        } else {
            for(auto&a:applications) {
                Internship* intern = findInternship(a.getInternshipID());
                if(intern && intern->getCompanyID()==currentUserID) myApps.push_back(&a);
            }
        }
        if(myApps.empty()) { ConsoleUI::printInfo("You have no applications to view."); ConsoleUI::pressEnter(); return; }

        // Show numbered list
        cout << Color::CYAN << "  Pick an application to see its full details:\n\n";
        for(size_t i=0; i<myApps.size(); i++) {
            Internship* intern = findInternship(myApps[i]->getInternshipID());
            Company* comp = intern ? findCompany(intern->getCompanyID()) : nullptr;
            Student* stu = findStudent(myApps[i]->getStudentID());
            cout << Color::BYELLOW << "    [" << (i+1) << "] " << Color::WHITE;
            if(currentUserType=="student")
                cout << (intern?intern->getRole():"?") << " at " << (comp?comp->getName():"?");
            else
                cout << (stu?stu->getName():"?") << " -> " << (intern?intern->getRole():"?");
            cout << Color::DIM << "  (" << myApps[i]->getStatusColor() << myApps[i]->getStatus() << Color::DIM << ", " << myApps[i]->getDate() << ")" << Color::RESET << "\n";
        }

        int sel = ConsoleUI::getInt("Enter the number of the application you want to view", 1, (int)myApps.size()) - 1;
        Application* app = myApps[sel];
        Internship* intern = findInternship(app->getInternshipID());
        Company* comp = intern ? findCompany(intern->getCompanyID()) : nullptr;
        Student* stu = findStudent(app->getStudentID());

        ConsoleUI::printSubHeader("Application Details");
        cout << Color::CYAN << "  Student     : " << Color::WHITE << (stu?stu->getName():"?") << "\n";
        cout << Color::CYAN << "  Role        : " << Color::WHITE << (intern?intern->getRole():"?") << "\n";
        cout << Color::CYAN << "  Company     : " << Color::WHITE << (comp?comp->getName():"?") << "\n";
        cout << Color::CYAN << "  Applied On  : " << Color::WHITE << app->getDate() << "\n";
        cout << Color::CYAN << "  Resume Score: " << Color::WHITE << app->getResumeScore() << "/100\n";
        cout << Color::CYAN << "  Skill Match : "; ConsoleUI::printProgressBar(app->getMatchPercentage(), 100); cout << "\n";
        cout << Color::CYAN << "  Status      : " << app->getStatusColor() << app->getStatus() << Color::RESET << "\n";
        if(!app->getRemarks().empty()) cout << Color::CYAN << "  Remarks     : " << Color::WHITE << app->getRemarks() << "\n";
        if(stu && intern) {
            auto missing = ResumeScorer::getMissingSkills(*stu, *intern);
            if(!missing.empty()) ConsoleUI::printInfo("Skills the student is missing: " + StrUtil::join(missing, ", "));
        }
        cout << Color::RESET;
        ConsoleUI::pressEnter();
    }

    // ============ REGISTRATION ============
    void registerStudent() {
        ConsoleUI::printHeader("STUDENT REGISTRATION");
        ConsoleUI::printInfo("Please fill in the following details to create your student account.");
        cout << "\n";
        string name = ConsoleUI::getInput("Your Full Name (e.g. Priya Sharma)");
        string email = ConsoleUI::getInput("Your Email Address (e.g. priya@gmail.com)");
        for(auto&s:students) { if(s.getEmail()==email) { ConsoleUI::printError("This email is already registered! Please login instead."); ConsoleUI::pressEnter(); return; } }
        string pwd = ConsoleUI::getInput("Create a Password (minimum 4 characters)");
        if(pwd.size() < 4) { ConsoleUI::printError("Password too short! Must be at least 4 characters."); ConsoleUI::pressEnter(); return; }
        float cgpa = ConsoleUI::getFloat("Your Current CGPA (on a scale of 0.0 to 10.0)", 0, 10);
        string sk = ConsoleUI::getInput("Your Skills (separate each skill with a comma, e.g. Python, C++, SQL, Machine Learning)");
        vector<string> skills = StrUtil::split(sk, ',');
        string resume = ConsoleUI::getInput("Write a short description about yourself (your experience, projects, interests)");
        Student s(nextStudentID(), name, email, StrUtil::hashPassword(pwd), cgpa, skills, resume);
        students.push_back(s);
        saveAll();
        ConsoleUI::printSuccess("Registration successful! Welcome, " + name + "!");
        ConsoleUI::printInfo("You can now login using your email and password.");
        ConsoleUI::pressEnter();
    }

    void registerCompany() {
        ConsoleUI::printHeader("COMPANY REGISTRATION");
        ConsoleUI::printInfo("Please fill in the following details to register your company.");
        cout << "\n";
        string name = ConsoleUI::getInput("Company Name (e.g. TechNova Solutions)");
        string email = ConsoleUI::getInput("Company Email Address (e.g. hr@technova.com)");
        for(auto&c:companies) { if(c.getEmail()==email) { ConsoleUI::printError("This email is already registered! Please login instead."); ConsoleUI::pressEnter(); return; } }
        string pwd = ConsoleUI::getInput("Create a Password (minimum 4 characters)");
        if(pwd.size() < 4) { ConsoleUI::printError("Password too short! Must be at least 4 characters."); ConsoleUI::pressEnter(); return; }
        string industry = ConsoleUI::getInput("Industry Type (choose one: IT / Finance / Consulting / Manufacturing / Other)");
        Company c(nextCompanyID(), name, email, StrUtil::hashPassword(pwd), industry);
        companies.push_back(c);
        saveAll();
        ConsoleUI::printSuccess("Registration successful! Welcome, " + name + "!");
        ConsoleUI::printInfo("You can now login using your email and password.");
        ConsoleUI::pressEnter();
    }

    // ============ LOGIN ============
    int loginStudent() {
        ConsoleUI::printHeader("STUDENT LOGIN");
        string email = ConsoleUI::getInput("Enter your registered email address");
        string pwd = ConsoleUI::getInput("Enter your password");
        for(auto&s:students) {
            if(s.getEmail()==email && s.checkPassword(pwd)) {
                currentUserID = s.getId(); currentUserType = "student";
                ConsoleUI::printSuccess("Welcome back, " + s.getName() + "!");
                return s.getId();
            }
        }
        ConsoleUI::printError("Wrong email or password! Please try again."); ConsoleUI::pressEnter(); return -1;
    }

    int loginCompany() {
        ConsoleUI::printHeader("COMPANY LOGIN");
        string email = ConsoleUI::getInput("Enter your company's registered email address");
        string pwd = ConsoleUI::getInput("Enter your password");
        for(auto&c:companies) {
            if(c.getEmail()==email && c.checkPassword(pwd)) {
                currentUserID = c.getId(); currentUserType = "company";
                ConsoleUI::printSuccess("Welcome back, " + c.getName() + "!");
                return c.getId();
            }
        }
        ConsoleUI::printError("Wrong email or password! Please try again."); ConsoleUI::pressEnter(); return -1;
    }

    // ============ POST INTERNSHIP (Company) ============
    void postInternship() {
        ConsoleUI::printHeader("POST A NEW INTERNSHIP OPPORTUNITY");
        Company* comp = findCompany(currentUserID);
        if(!comp) return;
        ConsoleUI::printInfo("Fill in the details below to create a new internship listing.");
        cout << "\n";
        string role = ConsoleUI::getInput("Job Title / Role (e.g. Software Developer Intern, Data Analyst Intern)");
        string sk = ConsoleUI::getInput("Required Skills (separate with commas, e.g. Python, SQL, Machine Learning)");
        vector<string> skills = StrUtil::split(sk, ',');
        float minCGPA = ConsoleUI::getFloat("Minimum CGPA required (0.0 to 10.0)", 0, 10);
        int stipend = ConsoleUI::getInt("Monthly Stipend in Rupees (e.g. 25000)", 0, 999999);
        int duration = ConsoleUI::getInt("Internship Duration in months (e.g. 3)", 1, 24);
        string location = ConsoleUI::getInput("Work Location (e.g. Bangalore, Remote, Mumbai)");
        string deadline;
        while(true) {
            deadline = ConsoleUI::getInput("Last date to apply (format: DD-MM-YYYY, e.g. 30-06-2026)");
            if(DateUtil::isValidDate(deadline) && !DateUtil::isDeadlinePassed(deadline)) break;
            ConsoleUI::printError("That date is invalid or already passed. Please enter a future date in DD-MM-YYYY format.");
        }
        int maxApp = ConsoleUI::getInt("Maximum number of applicants you want to accept (e.g. 50)", 1, 10000);
        int iid = nextInternshipID();
        Internship intern(iid, currentUserID, role, skills, minCGPA, stipend, duration, location, deadline, maxApp);
        internships.push_back(intern);
        comp->addInternship(iid);
        saveAll();
        ConsoleUI::printSuccess("Internship '" + role + "' posted successfully!");
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
        ConsoleUI::printHeader("BROWSE AVAILABLE INTERNSHIPS");
        cout << Color::CYAN << "  How would you like to view internships?\n\n";
        ConsoleUI::printMenuItem(1, "Show All Open Internships");
        ConsoleUI::printMenuItem(2, "Show Only Those With Stipend Above a Certain Amount");
        ConsoleUI::printMenuItem(3, "Show Only in a Specific City/Location");
        ConsoleUI::printMenuItem(4, "Show Only Those Requiring a Specific Skill");
        ConsoleUI::printMenuItem(5, "Sort by Highest Stipend First");
        ConsoleUI::printMenuItem(6, "Sort by Nearest Deadline First");
        ConsoleUI::printMenuItem(7, "Sort by Best Match for Your Profile First");
        int ch = ConsoleUI::getChoice(1, 7);

        vector<Internship> filtered;
        for(auto&i:internships) { i.autoClose(); if(i.isOpen()) filtered.push_back(i); }

        if(ch==2) {
            int minS = ConsoleUI::getInt("Enter the minimum stipend amount in Rupees (e.g. 20000)", 0, 999999);
            filtered = filterItems<Internship>(filtered, [minS](const Internship&i){ return i.getStipend()>=minS; });
        } else if(ch==3) {
            string loc = ConsoleUI::getInput("Enter a city or location name (e.g. Bangalore, Remote)");
            filtered = filterItems<Internship>(filtered, [loc](const Internship&i){ return StrUtil::toLower(i.getLocation()).find(StrUtil::toLower(loc))!=string::npos; });
        } else if(ch==4) {
            string sk = ConsoleUI::getInput("Enter a skill to search for (e.g. Python, React, SQL)");
            filtered = filterItems<Internship>(filtered, [sk](const Internship&i){
                for(auto&s:i.getRequiredSkills()) if(StrUtil::toLower(s).find(StrUtil::toLower(sk))!=string::npos) return true;
                return false;
            });
        }

        Student* stu = findStudent(currentUserID);
        if(ch==5) sort(filtered.begin(), filtered.end(), [](auto&a,auto&b){ return a.getStipend()>b.getStipend(); });
        else if(ch==6) sort(filtered.begin(), filtered.end(), [](auto&a,auto&b){ return DateUtil::daysRemaining(a.getDeadline())<DateUtil::daysRemaining(b.getDeadline()); });
        else if(ch==7 && stu) sort(filtered.begin(), filtered.end(), [&](auto&a,auto&b){ return ResumeScorer::calculateMatchPercentage(*stu,a)>ResumeScorer::calculateMatchPercentage(*stu,b); });

        if(filtered.empty()) { ConsoleUI::printInfo("No internships found with the selected filter."); ConsoleUI::pressEnter(); return; }

        ConsoleUI::printSubHeader("Showing " + to_string(filtered.size()) + " Internship(s)");
        for(size_t idx=0; idx<filtered.size(); idx++) {
            auto& intern = filtered[idx];
            Company* c = findCompany(intern.getCompanyID());
            cout << Color::BOLD << Color::BMAGENTA << "  [" << (idx+1) << "] ";
            if(c) { cout << c->getName() << " "; ConsoleUI::printStars(c->getRating()); }
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
        ConsoleUI::printHeader("APPLY TO AN INTERNSHIP");
        Student* stu = findStudent(currentUserID);
        if(!stu) return;

        // Collect open internships the student hasn't already applied to
        vector<Internship*> available;
        for(auto&i:internships) {
            i.autoClose();
            if(!i.isOpen()) continue;
            bool alreadyApplied = false;
            for(auto&a:applications) {
                if(a.getStudentID()==currentUserID && a.getInternshipID()==i.getId()) { alreadyApplied=true; break; }
            }
            if(!alreadyApplied) available.push_back(&i);
        }

        if(available.empty()) {
            ConsoleUI::printInfo("There are no open internships available to apply to right now (or you've already applied to all of them).");
            ConsoleUI::pressEnter(); return;
        }

        // Show numbered list of available internships with company names
        ConsoleUI::printSubHeader("Available Internships You Can Apply To");
        for(size_t idx=0; idx<available.size(); idx++) {
            Company* c = findCompany(available[idx]->getCompanyID());
            int mp = ResumeScorer::calculateMatchPercentage(*stu, *available[idx]);
            cout << Color::BOLD << Color::BMAGENTA << "  [" << (idx+1) << "] " << Color::RESET;
            cout << Color::BOLD << Color::WHITE << available[idx]->getRole() << Color::RESET;
            if(c) cout << Color::DIM << " at " << Color::BBLUE << c->getName() << Color::RESET;
            cout << Color::CYAN << "  |  Stipend: " << Color::BGREEN << "Rs." << available[idx]->getStipend() << "/month" << Color::RESET;
            cout << Color::CYAN << "  |  Location: " << Color::WHITE << available[idx]->getLocation() << Color::RESET;
            cout << Color::CYAN << "  |  Match: " << Color::WHITE << mp << "%" << Color::RESET;
            cout << Color::CYAN << "  |  Deadline: " << Color::WHITE << available[idx]->getDeadline() << Color::RESET;
            int dr = DateUtil::daysRemaining(available[idx]->getDeadline());
            if(dr > 0) cout << Color::BGREEN << " (" << dr << " days left)";
            cout << Color::RESET << "\n";
            // Show required skills
            cout << Color::DIM << "      Skills needed: " << Color::BYELLOW << StrUtil::join(available[idx]->getRequiredSkills(), ", ") << Color::RESET << "\n";
            ConsoleUI::printDivider();
        }

        int sel = ConsoleUI::getInt("Enter the number of the internship you want to apply to", 1, (int)available.size()) - 1;
        Internship* intern = available[sel];
        int iid = intern->getId();

        // Conflict: already selected elsewhere
        for(auto&a:applications) {
            if(a.getStudentID()==currentUserID && a.getStatus()=="SELECTED") {
                Internship* selIntern = findInternship(a.getInternshipID());
                Company* selComp = selIntern ? findCompany(selIntern->getCompanyID()) : nullptr;
                ConsoleUI::printWarning("Heads up: You are already SELECTED for '" + (selIntern?selIntern->getRole():"?") + "' at " + (selComp?selComp->getName():"?") + ". You can still apply here.");
            }
        }
        // CGPA warning
        if(stu->getCgpa() < intern->getMinCGPA()) {
            ConsoleUI::printWarning("Your CGPA (" + to_string(stu->getCgpa()).substr(0,3) + ") is below the minimum required (" + to_string(intern->getMinCGPA()).substr(0,3) + ").");
            if(!ConsoleUI::confirm("Do you still want to apply?")) return;
        }

        // Confirm application
        Company* comp = findCompany(intern->getCompanyID());
        cout << "\n";
        ConsoleUI::printInfo("You are about to apply for: " + intern->getRole() + " at " + (comp?comp->getName():"?"));
        if(!ConsoleUI::confirm("Confirm your application?")) return;

        int score = ResumeScorer::calculateScore(*stu, *intern);
        int matchPct = ResumeScorer::calculateMatchPercentage(*stu, *intern);
        int aid = nextAppID();
        Application app(aid, currentUserID, iid, DateUtil::getCurrentDate(), score, matchPct);
        applications.push_back(app);
        intern->incrementApplicants();
        if(intern->getCurrentApplicants() >= intern->getMaxApplicants()) intern->setStatus("CLOSED");
        stu->addApplication(aid);
        addNotif(intern->getCompanyID(), "company", "New application for " + intern->getRole() + " from " + stu->getName());
        // Show result
        auto missing = ResumeScorer::getMissingSkills(*stu, *intern);
        cout << "\n";
        ConsoleUI::printSuccess("Application submitted successfully!");
        cout << Color::CYAN << "  Role Applied For : " << Color::WHITE << intern->getRole() << "\n";
        cout << Color::CYAN << "  Company          : " << Color::WHITE << (comp?comp->getName():"?") << "\n";
        cout << Color::CYAN << "  Resume Score     : " << Color::WHITE << score << "/100\n";
        cout << Color::CYAN << "  Skill Match      : "; ConsoleUI::printProgressBar(matchPct, 100); cout << "\n";
        if(!missing.empty()) {
            ConsoleUI::printInfo("Skills you could improve to boost your chances: " + StrUtil::join(missing, ", "));
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
        ConsoleUI::printHeader("MANAGE APPLICANTS");
        Company* comp = findCompany(currentUserID);
        if(!comp) return;
        // List company's internships
        vector<Internship*> myInterns;
        for(auto&i:internships) if(i.getCompanyID()==currentUserID) myInterns.push_back(&i);
        if(myInterns.empty()) { ConsoleUI::printInfo("You haven't posted any internships yet. Post one first from the main menu."); ConsoleUI::pressEnter(); return; }
        cout << Color::CYAN << "  Select one of your internships to view its applicants:\n\n";
        for(size_t i=0;i<myInterns.size();i++) {
            int appCount = 0;
            for(auto&a:applications) if(a.getInternshipID()==myInterns[i]->getId()) appCount++;
            cout << Color::BYELLOW << "    [" << (i+1) << "] " << Color::WHITE << myInterns[i]->getRole()
                 << Color::DIM << " (" << appCount << " applicant(s), Status: "
                 << (myInterns[i]->isOpen() ? Color::BGREEN+"OPEN" : Color::BRED+"CLOSED") << Color::DIM << ")" << Color::RESET << "\n";
        }
        int sel = ConsoleUI::getInt("Enter the number of the internship", 1, (int)myInterns.size()) - 1;
        int iid = myInterns[sel]->getId();

        ConsoleUI::printSubHeader("Applicants for: " + myInterns[sel]->getRole());
        cout << Color::CYAN << "  What would you like to do?\n\n";
        ConsoleUI::printMenuItem(1, "View All Applicants");
        ConsoleUI::printMenuItem(2, "Auto-Shortlist Top 30% of Applicants");
        ConsoleUI::printMenuItem(3, "Show Only Applicants With CGPA Above a Certain Value");
        ConsoleUI::printMenuItem(4, "Accept / Reject / Shortlist a Specific Applicant");
        ConsoleUI::printMenuItem(5, "Bulk Reject All Applicants Below a Certain Score");
        int action = ConsoleUI::getChoice(1, 5);

        if(action==2) {
            auto shortlisted = MatchEngine::autoShortlist(applications, iid);
            ConsoleUI::printSuccess("Auto-shortlisted " + to_string(shortlisted.size()) + " applicant(s) based on their resume scores!");
            for(int sid : shortlisted) {
                addNotif(sid, "student", "Your application for " + myInterns[sel]->getRole() + " has been SHORTLISTED!");
            }
            saveAll();
        }

        if(action==4) {
            // Show numbered list of applicants for this internship
            vector<Application*> internApps;
            for(auto&a:applications) if(a.getInternshipID()==iid) internApps.push_back(&a);
            if(internApps.empty()) { ConsoleUI::printInfo("No one has applied to this internship yet."); ConsoleUI::pressEnter(); return; }

            cout << Color::CYAN << "\n  Select the applicant whose status you want to change:\n\n";
            for(size_t i=0; i<internApps.size(); i++) {
                Student* s = findStudent(internApps[i]->getStudentID());
                cout << Color::BYELLOW << "    [" << (i+1) << "] " << Color::WHITE << (s?s->getName():"Unknown")
                     << Color::DIM << " | CGPA: " << (s?to_string(s->getCgpa()).substr(0,3):"?") 
                     << " | Score: " << internApps[i]->getResumeScore()
                     << " | Match: " << internApps[i]->getMatchPercentage() << "%"
                     << " | Current Status: " << internApps[i]->getStatusColor() << internApps[i]->getStatus()
                     << Color::RESET << "\n";
            }

            int appSel = ConsoleUI::getInt("Enter the number of the applicant", 1, (int)internApps.size()) - 1;
            Application* app = internApps[appSel];
            Student* selectedStu = findStudent(app->getStudentID());
            cout << "\n" << Color::CYAN << "  What do you want to do with " << Color::WHITE << (selectedStu?selectedStu->getName():"this applicant") << Color::CYAN << "'s application?\n\n";
            ConsoleUI::printMenuItem(1, "Shortlist (move to next round)");
            ConsoleUI::printMenuItem(2, "Reject (decline this applicant)");
            ConsoleUI::printMenuItem(3, "Select (offer the internship to this applicant)");
            int st = ConsoleUI::getChoice(1, 3);
            string statuses[] = {"SHORTLISTED", "REJECTED", "SELECTED"};
            app->setStatus(statuses[st-1]);
            string remarks = ConsoleUI::getInput("Add any remarks or feedback for this applicant (or press Enter to skip)");
            if(!remarks.empty()) app->setRemarks(remarks);
            if(st==3) comp->incrementHires();
            addNotif(app->getStudentID(), "student", "Application for " + myInterns[sel]->getRole() + ": " + statuses[st-1]);
            saveAll();
            ConsoleUI::printSuccess("Status updated to " + statuses[st-1] + " for " + (selectedStu?selectedStu->getName():"applicant") + "!");
            ConsoleUI::pressEnter(); return;
        }

        if(action==5) {
            int minScore = ConsoleUI::getInt("Enter the minimum resume score — all applicants scoring below this will be rejected (0-100)", 0, 100);
            int cnt=0;
            for(auto&a:applications) {
                if(a.getInternshipID()==iid && a.getStatus()=="APPLIED" && a.getResumeScore()<minScore) {
                    a.setStatus("REJECTED"); cnt++;
                    addNotif(a.getStudentID(), "student", "Application for " + myInterns[sel]->getRole() + ": REJECTED");
                }
            }
            ConsoleUI::printSuccess("Rejected " + to_string(cnt) + " applicant(s) with resume score below " + to_string(minScore) + ".");
            saveAll(); ConsoleUI::pressEnter(); return;
        }

        // Display applicants table
        float minCGPA = 0;
        if(action==3) minCGPA = ConsoleUI::getFloat("Enter the minimum CGPA to filter by (e.g. 7.5)", 0, 10);
        vector<string> hdr = {"#", "Student Name", "CGPA", "Score", "Match%", "Status"};
        vector<int> widths = {5, 22, 7, 7, 8, 13};
        ConsoleUI::printTableHeader(hdr, widths);
        int rowNum = 0;
        for(auto&a:applications) {
            if(a.getInternshipID()!=iid) continue;
            Student* s = findStudent(a.getStudentID());
            if(!s) continue;
            if(action==3 && s->getCgpa()<minCGPA) continue;
            rowNum++;
            vector<string> row = {to_string(rowNum), s->getName(),
                to_string(s->getCgpa()).substr(0,3), to_string(a.getResumeScore()),
                to_string(a.getMatchPercentage())+"%", a.getStatusColor()+a.getStatus()+Color::RESET};
            ConsoleUI::printTableRow(row, widths);
        }
        if(rowNum==0) ConsoleUI::printInfo("No applicants found with the selected filter.");
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
        ConsoleUI::printInfo("You can rate companies you have interacted with (applied, shortlisted, selected, or rejected).");
        cout << "\n";
        // Show companies student has interacted with
        vector<int> compIDList;
        set<int> compIDSet;
        for(auto&a:applications) {
            if(a.getStudentID()!=currentUserID) continue;
            if(a.getStatus()=="SHORTLISTED" || a.getStatus()=="SELECTED" || a.getStatus()=="REJECTED") {
                Internship* intern = findInternship(a.getInternshipID());
                if(intern && !compIDSet.count(intern->getCompanyID())) {
                    compIDSet.insert(intern->getCompanyID());
                    compIDList.push_back(intern->getCompanyID());
                }
            }
        }
        if(compIDList.empty()) { ConsoleUI::printInfo("You haven't interacted with any companies yet. Apply to internships first!"); ConsoleUI::pressEnter(); return; }
        cout << Color::CYAN << "  Select a company to rate:\n\n";
        for(size_t i=0; i<compIDList.size(); i++) {
            Company* c = findCompany(compIDList[i]);
            if(c) {
                cout << Color::BYELLOW << "    [" << (i+1) << "] " << Color::WHITE << c->getName() << " ";
                ConsoleUI::printStars(c->getRating());
                cout << Color::DIM << " (" << fixed << setprecision(1) << c->getRating() << "/5.0)" << Color::RESET << "\n";
            }
        }
        int sel = ConsoleUI::getInt("Enter the number of the company you want to rate", 1, (int)compIDList.size()) - 1;
        int cid = compIDList[sel];
        Company* c = findCompany(cid);
        cout << "\n" << Color::CYAN << "  How would you rate " << Color::WHITE << c->getName() << Color::CYAN << "?\n";
        cout << Color::DIM << "  (1 = Poor, 2 = Below Average, 3 = Average, 4 = Good, 5 = Excellent)\n" << Color::RESET;
        int stars = ConsoleUI::getInt("Your Rating (1-5)", 1, 5);
        c->addRating(stars);
        saveAll();
        ConsoleUI::printSuccess("You rated " + c->getName() + " with " + to_string(stars) + " star(s)! Thank you for your feedback.");
        ConsoleUI::pressEnter();
    }

    // ============ EDIT PROFILE ============
    void editStudentProfile() {
        ConsoleUI::printHeader("EDIT YOUR PROFILE");
        Student* stu = findStudent(currentUserID);
        if(!stu) return;
        stu->displayProfile();
        cout << "\n" << Color::CYAN << "  What would you like to update?\n\n";
        ConsoleUI::printMenuItem(1, "Change Your Name");
        ConsoleUI::printMenuItem(2, "Update Your CGPA");
        ConsoleUI::printMenuItem(3, "Update Your Skills");
        ConsoleUI::printMenuItem(4, "Rewrite Your About / Resume Description");
        ConsoleUI::printMenuItem(5, "Change Your Email Address");
        ConsoleUI::printMenuItem(6, "Go Back (No Changes)");
        int ch = ConsoleUI::getChoice(1, 6);
        if(ch==1) stu->setName(ConsoleUI::getInput("Enter your new name"));
        else if(ch==2) stu->setCgpa(ConsoleUI::getFloat("Enter your updated CGPA (0.0 to 10.0)", 0, 10));
        else if(ch==3) { string sk=ConsoleUI::getInput("Enter your updated skills (separate with commas, e.g. Python, C++, SQL)"); stu->setSkills(StrUtil::split(sk,',')); }
        else if(ch==4) stu->setResumeText(ConsoleUI::getInput("Write your updated about / resume description"));
        else if(ch==5) stu->setEmail(ConsoleUI::getInput("Enter your new email address"));
        else return;
        saveAll();
        ConsoleUI::printSuccess("Your profile has been updated successfully!");
        ConsoleUI::pressEnter();
    }

    void editCompanyProfile() {
        ConsoleUI::printHeader("EDIT COMPANY PROFILE");
        Company* comp = findCompany(currentUserID);
        if(!comp) return;
        comp->displayProfile();
        cout << "\n" << Color::CYAN << "  What would you like to update?\n\n";
        ConsoleUI::printMenuItem(1, "Change Company Name");
        ConsoleUI::printMenuItem(2, "Change Industry Type");
        ConsoleUI::printMenuItem(3, "Change Email Address");
        ConsoleUI::printMenuItem(4, "Go Back (No Changes)");
        int ch = ConsoleUI::getChoice(1, 4);
        if(ch==1) comp->setName(ConsoleUI::getInput("Enter the new company name"));
        else if(ch==2) comp->setIndustry(ConsoleUI::getInput("Enter the new industry type (IT / Finance / Consulting / Manufacturing / Other)"));
        else if(ch==3) comp->setEmail(ConsoleUI::getInput("Enter the new email address"));
        else return;
        saveAll();
        ConsoleUI::printSuccess("Company profile updated successfully!");
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
            cout << "\n" << Color::CYAN << "  What would you like to do?\n\n";
            ConsoleUI::printMenuItem(1, "Browse Available Internships");
            ConsoleUI::printMenuItem(2, "Search Internships by Keyword");
            ConsoleUI::printMenuItem(3, "Apply to an Internship");
            ConsoleUI::printMenuItem(4, "View My Applications");
            ConsoleUI::printMenuItem(5, "View Application Details");
            ConsoleUI::printMenuItem(6, "My Dashboard & Analytics");
            ConsoleUI::printMenuItem(7, "Recommended Internships for Me");
            ConsoleUI::printMenuItem(8, "Notifications" + (unread>0 ? " ("+Color::BRED+to_string(unread)+" new"+Color::RESET+")" : ""));
            ConsoleUI::printMenuItem(9, "Rate a Company I've Interacted With");
            ConsoleUI::printMenuItem(10, "Edit My Profile");
            ConsoleUI::printMenuItem(11, "Change My Password");
            ConsoleUI::printMenuItem(12, "Export My Report");
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
            cout << "\n" << Color::CYAN << "  What would you like to do?\n\n";
            ConsoleUI::printMenuItem(1, "Post a New Internship Opportunity");
            ConsoleUI::printMenuItem(2, "View & Manage Applicants (Accept / Reject / Shortlist)");
            ConsoleUI::printMenuItem(3, "View Application Details");
            ConsoleUI::printMenuItem(4, "Company Dashboard & Analytics");
            ConsoleUI::printMenuItem(5, "See Recommended Candidates for Your Internships");
            ConsoleUI::printMenuItem(6, "Notifications" + (unread>0 ? " ("+Color::BRED+to_string(unread)+" new"+Color::RESET+")" : ""));
            ConsoleUI::printMenuItem(7, "Edit Company Profile");
            ConsoleUI::printMenuItem(8, "Change Password");
            ConsoleUI::printMenuItem(9, "Export Company Report");
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
            cout << Color::CYAN << "  Welcome! Please choose an option:\n\n";
            ConsoleUI::printMenuItem(1, "Login as Student");
            ConsoleUI::printMenuItem(2, "Login as Company");
            ConsoleUI::printMenuItem(3, "Register as a New Student");
            ConsoleUI::printMenuItem(4, "Register as a New Company");
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
