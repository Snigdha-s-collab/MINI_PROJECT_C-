#pragma once
#include "Utils.h"

// ======================================================================
//  BASE CLASS: User (Demonstrates INHERITANCE & POLYMORPHISM)
// ======================================================================
class User {
protected:
    int id;
    string name;
    string email;
    string passwordHash;

public:
    User() : id(0) {}
    User(int id, const string& name, const string& email, const string& pwdHash)
        : id(id), name(name), email(email), passwordHash(pwdHash) {}
    virtual ~User() {}

    // Pure virtual - demonstrates ABSTRACTION
    virtual string getRole() const = 0;
    virtual void displayDashboardHeader() const = 0;

    // Getters
    int getId() const { return id; }
    string getName() const { return name; }
    string getEmail() const { return email; }
    string getPasswordHash() const { return passwordHash; }

    // Setters
    void setName(const string& n) { name = n; }
    void setEmail(const string& e) { email = e; }
    void setPasswordHash(const string& h) { passwordHash = h; }

    bool checkPassword(const string& pwd) const {
        return passwordHash == StrUtil::hashPassword(pwd);
    }

    void changePassword(const string& newPwd) {
        passwordHash = StrUtil::hashPassword(newPwd);
    }

    // Operator overloading: ==
    bool operator==(const User& other) const { return id == other.id && email == other.email; }

    // Friend function for serialization
    friend ostream& operator<<(ostream& os, const User& u) {
        os << u.id << "|" << u.name << "|" << u.email << "|" << u.passwordHash;
        return os;
    }
};

// ======================================================================
//  STUDENT CLASS (Inherits from User)
// ======================================================================
class Student : public User {
private:
    float cgpa;
    vector<string> skills;
    string resumeText;
    vector<int> applicationHistory;
    int profileCompleteness;

    void calcCompleteness() {
        int score = 0;
        if (!name.empty()) score += 20;
        if (!email.empty()) score += 20;
        if (cgpa > 0) score += 20;
        if (!skills.empty()) score += 20;
        if (!resumeText.empty()) score += 20;
        profileCompleteness = score;
    }

public:
    Student() : User(), cgpa(0), profileCompleteness(0) {}

    Student(int id, const string& name, const string& email, const string& pwdHash,
            float cgpa, const vector<string>& skills, const string& resume)
        : User(id, name, email, pwdHash), cgpa(cgpa), skills(skills), resumeText(resume) {
        calcCompleteness();
    }

    // Override virtual functions - POLYMORPHISM
    string getRole() const override { return "Student"; }

    void displayDashboardHeader() const override {
        cout << Color::BGREEN << "  Student: " << Color::BOLD << name << Color::RESET
             << Color::DIM << " (ID: S" << id << ")" << Color::RESET << "\n";
        cout << Color::CYAN << "  CGPA: " << Color::WHITE << fixed << setprecision(1) << cgpa
             << Color::CYAN << "  |  Skills: " << Color::WHITE << skills.size()
             << Color::CYAN << "  |  Profile: ";
        ConsoleUI::printProgressBar(profileCompleteness, 100, 15);
        cout << "\n";
    }

    // Getters
    float getCgpa() const { return cgpa; }
    vector<string> getSkills() const { return skills; }
    string getResumeText() const { return resumeText; }
    vector<int> getApplicationHistory() const { return applicationHistory; }
    int getProfileCompleteness() const { return profileCompleteness; }

    // Setters
    void setCgpa(float c) { cgpa = c; calcCompleteness(); }
    void setSkills(const vector<string>& s) { skills = s; calcCompleteness(); }
    void setResumeText(const string& r) { resumeText = r; calcCompleteness(); }
    void addApplication(int appId) { applicationHistory.push_back(appId); }
    void setApplicationHistory(const vector<int>& h) { applicationHistory = h; }

    // Serialize / Deserialize
    string serialize() const {
        string s = to_string(id) + "|" + name + "|" + email + "|" + passwordHash + "|"
                   + to_string(cgpa) + "|" + StrUtil::join(skills, ",") + "|" + resumeText + "|";
        vector<string> appStrs;
        for (int a : applicationHistory) appStrs.push_back(to_string(a));
        s += StrUtil::join(appStrs, ",");
        return s;
    }

    static Student deserialize(const string& line) {
        vector<string> parts = StrUtil::split(line, '|');
        Student s;
        if (parts.size() >= 7) {
            s.id = stoi(parts[0]);
            s.name = parts[1];
            s.email = parts[2];
            s.passwordHash = parts[3];
            s.cgpa = stof(parts[4]);
            s.skills = StrUtil::split(parts[5], ',');
            s.resumeText = parts[6];
            if (parts.size() >= 8 && !parts[7].empty()) {
                vector<string> ids = StrUtil::split(parts[7], ',');
                for (auto& i : ids) {
                    try { s.applicationHistory.push_back(stoi(i)); } catch (...) {}
                }
            }
        }
        s.calcCompleteness();
        return s;
    }

    void displayProfile() const {
        ConsoleUI::printSubHeader("Student Profile");
        cout << Color::CYAN << "  ID        : " << Color::WHITE << "S" << id << "\n";
        cout << Color::CYAN << "  Name      : " << Color::WHITE << name << "\n";
        cout << Color::CYAN << "  Email     : " << Color::WHITE << email << "\n";
        cout << Color::CYAN << "  CGPA      : " << Color::WHITE << fixed << setprecision(1) << cgpa << "/10.0\n";
        cout << Color::CYAN << "  Skills    : " << Color::BGREEN;
        for (size_t i = 0; i < skills.size(); i++) {
            cout << skills[i];
            if (i < skills.size() - 1) cout << ", ";
        }
        if (skills.empty()) cout << "(none)";
        cout << "\n";
        cout << Color::CYAN << "  Resume    : " << Color::WHITE << (resumeText.empty() ? "(not set)" : resumeText.substr(0, 60) + "...") << "\n";
        cout << Color::CYAN << "  Profile   : ";
        ConsoleUI::printProgressBar(profileCompleteness, 100);
        cout << "\n" << Color::RESET;
    }

    // Export profile to string for report
    string exportReport() const {
        stringstream ss;
        ss << "=== Student Report ===\n";
        ss << "ID        : S" << id << "\n";
        ss << "Name      : " << name << "\n";
        ss << "Email     : " << email << "\n";
        ss << "CGPA      : " << fixed << setprecision(1) << cgpa << "/10.0\n";
        ss << "Skills    : " << StrUtil::join(skills, ", ") << "\n";
        ss << "Resume    : " << resumeText << "\n";
        ss << "Profile % : " << profileCompleteness << "%\n";
        ss << "Applications: " << applicationHistory.size() << "\n";
        return ss.str();
    }
};

// ======================================================================
//  COMPANY CLASS (Inherits from User)
// ======================================================================
class Company : public User {
private:
    string industry;
    float companyRating;
    int totalRatings;
    float ratingSum;
    vector<int> postedInternships;
    int hireHistory;

public:
    Company() : User(), companyRating(0), totalRatings(0), ratingSum(0), hireHistory(0) {}

    Company(int id, const string& name, const string& email, const string& pwdHash,
            const string& industry)
        : User(id, name, email, pwdHash), industry(industry),
          companyRating(0), totalRatings(0), ratingSum(0), hireHistory(0) {}

    string getRole() const override { return "Company"; }

    void displayDashboardHeader() const override {
        cout << Color::BBLUE << "  Company: " << Color::BOLD << name << Color::RESET
             << Color::DIM << " (ID: C" << id << ")" << Color::RESET << "\n";
        cout << Color::CYAN << "  Industry: " << Color::WHITE << industry
             << Color::CYAN << "  |  Rating: ";
        ConsoleUI::printStars(companyRating);
        cout << Color::DIM << " (" << fixed << setprecision(1) << companyRating << "/5.0)"
             << Color::CYAN << "  |  Hires: " << Color::WHITE << hireHistory << "\n" << Color::RESET;
    }

    // Getters
    string getIndustry() const { return industry; }
    float getRating() const { return companyRating; }
    int getTotalRatings() const { return totalRatings; }
    vector<int> getPostedInternships() const { return postedInternships; }
    int getHireHistory() const { return hireHistory; }

    // Setters
    void setIndustry(const string& i) { industry = i; }
    void addInternship(int iid) { postedInternships.push_back(iid); }
    void setPostedInternships(const vector<int>& v) { postedInternships = v; }
    void incrementHires() { hireHistory++; }
    void setHireHistory(int h) { hireHistory = h; }
    void setRatingData(float sum, int count) {
        ratingSum = sum;
        totalRatings = count;
        companyRating = (count > 0) ? sum / count : 0;
    }

    void addRating(int stars) {
        ratingSum += stars;
        totalRatings++;
        companyRating = ratingSum / totalRatings;
    }

    float getRatingSum() const { return ratingSum; }

    string serialize() const {
        string s = to_string(id) + "|" + name + "|" + email + "|" + passwordHash + "|"
                   + industry + "|" + to_string(companyRating) + "|" + to_string(totalRatings) + "|"
                   + to_string(ratingSum) + "|";
        vector<string> iids;
        for (int i : postedInternships) iids.push_back(to_string(i));
        s += StrUtil::join(iids, ",") + "|" + to_string(hireHistory);
        return s;
    }

    static Company deserialize(const string& line) {
        vector<string> parts = StrUtil::split(line, '|');
        Company c;
        if (parts.size() >= 10) {
            c.id = stoi(parts[0]);
            c.name = parts[1];
            c.email = parts[2];
            c.passwordHash = parts[3];
            c.industry = parts[4];
            c.companyRating = stof(parts[5]);
            c.totalRatings = stoi(parts[6]);
            c.ratingSum = stof(parts[7]);
            if (!parts[8].empty()) {
                vector<string> ids = StrUtil::split(parts[8], ',');
                for (auto& i : ids) {
                    try { c.postedInternships.push_back(stoi(i)); } catch (...) {}
                }
            }
            c.hireHistory = stoi(parts[9]);
        }
        return c;
    }

    void displayProfile() const {
        ConsoleUI::printSubHeader("Company Profile");
        cout << Color::CYAN << "  ID        : " << Color::WHITE << "C" << id << "\n";
        cout << Color::CYAN << "  Name      : " << Color::WHITE << name << "\n";
        cout << Color::CYAN << "  Email     : " << Color::WHITE << email << "\n";
        cout << Color::CYAN << "  Industry  : " << Color::WHITE << industry << "\n";
        cout << Color::CYAN << "  Rating    : ";
        ConsoleUI::printStars(companyRating);
        cout << " (" << fixed << setprecision(1) << companyRating << "/5.0, " << totalRatings << " reviews)\n";
        cout << Color::CYAN << "  Internships: " << Color::WHITE << postedInternships.size() << "\n";
        cout << Color::CYAN << "  Total Hires: " << Color::WHITE << hireHistory << "\n" << Color::RESET;
    }

    // Export profile to string for report
    string exportReport() const {
        stringstream ss;
        ss << "=== Company Report ===\n";
        ss << "ID         : C" << id << "\n";
        ss << "Name       : " << name << "\n";
        ss << "Email      : " << email << "\n";
        ss << "Industry   : " << industry << "\n";
        ss << "Rating     : " << fixed << setprecision(1) << companyRating << "/5.0 (" << totalRatings << " reviews)\n";
        ss << "Internships: " << postedInternships.size() << "\n";
        ss << "Total Hires: " << hireHistory << "\n";
        return ss.str();
    }
};

// ======================================================================
//  INTERNSHIP CLASS
// ======================================================================
class Internship {
private:
    int internshipID;
    int companyID;
    string role;
    vector<string> requiredSkills;
    float minimumCGPA;
    int stipend;
    int duration; // months
    string location;
    string applicationDeadline; // DD-MM-YYYY
    int maxApplicants;
    int currentApplicants;
    string status; // OPEN / CLOSED

public:
    Internship() : internshipID(0), companyID(0), minimumCGPA(0), stipend(0),
                   duration(0), maxApplicants(0), currentApplicants(0), status("OPEN") {}

    Internship(int id, int cid, const string& role, const vector<string>& skills,
               float minCGPA, int stip, int dur, const string& loc,
               const string& deadline, int maxApp)
        : internshipID(id), companyID(cid), role(role), requiredSkills(skills),
          minimumCGPA(minCGPA), stipend(stip), duration(dur), location(loc),
          applicationDeadline(deadline), maxApplicants(maxApp), currentApplicants(0), status("OPEN") {}

    // Getters
    int getId() const { return internshipID; }
    int getCompanyID() const { return companyID; }
    string getRole() const { return role; }
    vector<string> getRequiredSkills() const { return requiredSkills; }
    float getMinCGPA() const { return minimumCGPA; }
    int getStipend() const { return stipend; }
    int getDuration() const { return duration; }
    string getLocation() const { return location; }
    string getDeadline() const { return applicationDeadline; }
    int getMaxApplicants() const { return maxApplicants; }
    int getCurrentApplicants() const { return currentApplicants; }
    string getStatus() const { return status; }

    void incrementApplicants() { currentApplicants++; }
    void setStatus(const string& s) { status = s; }
    void setCurrentApplicants(int c) { currentApplicants = c; }

    bool isOpen() const {
        return status == "OPEN" && !DateUtil::isDeadlinePassed(applicationDeadline)
               && currentApplicants < maxApplicants;
    }

    void autoClose() {
        if (DateUtil::isDeadlinePassed(applicationDeadline) || currentApplicants >= maxApplicants)
            status = "CLOSED";
    }

    // Operator overloading for display
    friend ostream& operator<<(ostream& os, const Internship& i) {
        os << Color::BOLD << Color::WHITE << "  " << i.role << Color::RESET
           << Color::DIM << " (I" << i.internshipID << ")" << Color::RESET << "\n";
        os << Color::CYAN << "    Stipend: " << Color::BGREEN << "Rs." << i.stipend << "/month"
           << Color::CYAN << "  |  Duration: " << Color::WHITE << i.duration << " months"
           << Color::CYAN << "  |  Location: " << Color::WHITE << i.location << "\n";
        os << Color::CYAN << "    Min CGPA: " << Color::WHITE << fixed << setprecision(1) << i.minimumCGPA
           << Color::CYAN << "  |  Skills: " << Color::BYELLOW;
        for (size_t k = 0; k < i.requiredSkills.size(); k++) {
            os << i.requiredSkills[k];
            if (k < i.requiredSkills.size() - 1) os << ", ";
        }
        os << "\n";
        os << Color::CYAN << "    Deadline: " << Color::WHITE << i.applicationDeadline;
        int dr = DateUtil::daysRemaining(i.applicationDeadline);
        if (dr > 0) os << Color::BGREEN << " (" << dr << " days left)";
        else os << Color::BRED << " (EXPIRED)";
        os << Color::CYAN << "  |  Applicants: " << Color::WHITE << i.currentApplicants << "/" << i.maxApplicants;
        os << Color::CYAN << "  |  Status: ";
        if (i.isOpen()) os << Color::BGREEN << "OPEN";
        else os << Color::BRED << "CLOSED";
        os << Color::RESET << "\n";
        return os;
    }

    string serialize() const {
        return to_string(internshipID) + "|" + to_string(companyID) + "|" + role + "|"
               + StrUtil::join(requiredSkills, ",") + "|" + to_string(minimumCGPA) + "|"
               + to_string(stipend) + "|" + to_string(duration) + "|" + location + "|"
               + applicationDeadline + "|" + to_string(maxApplicants) + "|"
               + to_string(currentApplicants) + "|" + status;
    }

    static Internship deserialize(const string& line) {
        vector<string> p = StrUtil::split(line, '|');
        Internship i;
        if (p.size() >= 12) {
            i.internshipID = stoi(p[0]);
            i.companyID = stoi(p[1]);
            i.role = p[2];
            i.requiredSkills = StrUtil::split(p[3], ',');
            i.minimumCGPA = stof(p[4]);
            i.stipend = stoi(p[5]);
            i.duration = stoi(p[6]);
            i.location = p[7];
            i.applicationDeadline = p[8];
            i.maxApplicants = stoi(p[9]);
            i.currentApplicants = stoi(p[10]);
            i.status = p[11];
        }
        return i;
    }
};

// ======================================================================
//  APPLICATION CLASS
// ======================================================================
class Application {
private:
    int applicationID;
    int studentID;
    int internshipID;
    string applicationDate;
    string status; // APPLIED, SHORTLISTED, REJECTED, SELECTED
    int resumeScore;
    int matchPercentage;
    string companyRemarks;

public:
    Application() : applicationID(0), studentID(0), internshipID(0),
                    status("APPLIED"), resumeScore(0), matchPercentage(0) {}

    Application(int aid, int sid, int iid, const string& date, int score, int match)
        : applicationID(aid), studentID(sid), internshipID(iid),
          applicationDate(date), status("APPLIED"), resumeScore(score),
          matchPercentage(match) {}

    // Getters
    int getId() const { return applicationID; }
    int getStudentID() const { return studentID; }
    int getInternshipID() const { return internshipID; }
    string getDate() const { return applicationDate; }
    string getStatus() const { return status; }
    int getResumeScore() const { return resumeScore; }
    int getMatchPercentage() const { return matchPercentage; }
    string getRemarks() const { return companyRemarks; }

    // Setters
    void setStatus(const string& s) { status = s; }
    void setResumeScore(int s) { resumeScore = s; }
    void setMatchPercentage(int m) { matchPercentage = m; }
    void setRemarks(const string& r) { companyRemarks = r; }

    string getStatusColor() const {
        if (status == "SELECTED") return Color::BGREEN;
        if (status == "SHORTLISTED") return Color::BCYAN;
        if (status == "REJECTED") return Color::BRED;
        return Color::BYELLOW;
    }

    string serialize() const {
        return to_string(applicationID) + "|" + to_string(studentID) + "|" + to_string(internshipID)
               + "|" + applicationDate + "|" + status + "|" + to_string(resumeScore) + "|"
               + to_string(matchPercentage) + "|" + companyRemarks;
    }

    static Application deserialize(const string& line) {
        vector<string> p = StrUtil::split(line, '|');
        Application a;
        if (p.size() >= 7) {
            a.applicationID = stoi(p[0]);
            a.studentID = stoi(p[1]);
            a.internshipID = stoi(p[2]);
            a.applicationDate = p[3];
            a.status = p[4];
            a.resumeScore = stoi(p[5]);
            a.matchPercentage = stoi(p[6]);
            if (p.size() >= 8) a.companyRemarks = p[7];
        }
        return a;
    }

    bool operator==(const Application& other) const { return applicationID == other.applicationID; }
};

// ======================================================================
//  NOTIFICATION CLASS
// ======================================================================
class Notification {
private:
    int notifID;
    int userID;
    string userType; // "student" or "company"
    string message;
    string timestamp;
    bool isRead;

public:
    Notification() : notifID(0), userID(0), isRead(false) {}

    Notification(int nid, int uid, const string& utype, const string& msg)
        : notifID(nid), userID(uid), userType(utype), message(msg),
          timestamp(DateUtil::getCurrentTimestamp()), isRead(false) {}

    int getId() const { return notifID; }
    int getUserID() const { return userID; }
    string getUserType() const { return userType; }
    string getMessage() const { return message; }
    string getTimestamp() const { return timestamp; }
    bool getIsRead() const { return isRead; }
    void markRead() { isRead = true; }

    string serialize() const {
        return to_string(notifID) + "|" + to_string(userID) + "|" + userType + "|"
               + message + "|" + timestamp + "|" + (isRead ? "1" : "0");
    }

    static Notification deserialize(const string& line) {
        vector<string> p = StrUtil::split(line, '|');
        Notification n;
        if (p.size() >= 6) {
            n.notifID = stoi(p[0]);
            n.userID = stoi(p[1]);
            n.userType = p[2];
            n.message = p[3];
            n.timestamp = p[4];
            n.isRead = (p[5] == "1");
        }
        return n;
    }
};
