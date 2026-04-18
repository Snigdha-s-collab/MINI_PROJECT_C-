#pragma once
#include "Models.h"

// ======================================================================
//  FILE MANAGER - Handles all data persistence
// ======================================================================
class FileManager {
private:
    string dataDir;

    void ensureDir() {
#ifdef _WIN32
        system(("mkdir \"" + dataDir + "\" 2>nul").c_str());
#else
        system(("mkdir -p \"" + dataDir + "\"").c_str());
#endif
    }

public:
    FileManager(const string& dir = "data") : dataDir(dir) { ensureDir(); }

    // ---- STUDENTS ----
    void saveStudents(const vector<Student>& students) {
        ofstream f(dataDir + "/students.dat", ios::trunc);
        for (auto& s : students) f << s.serialize() << "\n";
        f.close();
    }

    vector<Student> loadStudents() {
        vector<Student> v;
        ifstream f(dataDir + "/students.dat");
        string line;
        while (getline(f, line)) {
            if (!line.empty()) {
                try { v.push_back(Student::deserialize(line)); } catch (...) {}
            }
        }
        f.close();
        return v;
    }

    // ---- COMPANIES ----
    void saveCompanies(const vector<Company>& companies) {
        ofstream f(dataDir + "/companies.dat", ios::trunc);
        for (auto& c : companies) f << c.serialize() << "\n";
        f.close();
    }

    vector<Company> loadCompanies() {
        vector<Company> v;
        ifstream f(dataDir + "/companies.dat");
        string line;
        while (getline(f, line)) {
            if (!line.empty()) {
                try { v.push_back(Company::deserialize(line)); } catch (...) {}
            }
        }
        f.close();
        return v;
    }

    // ---- INTERNSHIPS ----
    void saveInternships(const vector<Internship>& internships) {
        ofstream f(dataDir + "/internships.dat", ios::trunc);
        for (auto& i : internships) f << i.serialize() << "\n";
        f.close();
    }

    vector<Internship> loadInternships() {
        vector<Internship> v;
        ifstream f(dataDir + "/internships.dat");
        string line;
        while (getline(f, line)) {
            if (!line.empty()) {
                try { v.push_back(Internship::deserialize(line)); } catch (...) {}
            }
        }
        f.close();
        return v;
    }

    // ---- APPLICATIONS ----
    void saveApplications(const vector<Application>& apps) {
        ofstream f(dataDir + "/applications.dat", ios::trunc);
        for (auto& a : apps) f << a.serialize() << "\n";
        f.close();
    }

    vector<Application> loadApplications() {
        vector<Application> v;
        ifstream f(dataDir + "/applications.dat");
        string line;
        while (getline(f, line)) {
            if (!line.empty()) {
                try { v.push_back(Application::deserialize(line)); } catch (...) {}
            }
        }
        f.close();
        return v;
    }

    // ---- NOTIFICATIONS ----
    void saveNotifications(const vector<Notification>& notifs) {
        ofstream f(dataDir + "/notifications.dat", ios::trunc);
        for (auto& n : notifs) f << n.serialize() << "\n";
        f.close();
    }

    vector<Notification> loadNotifications() {
        vector<Notification> v;
        ifstream f(dataDir + "/notifications.dat");
        string line;
        while (getline(f, line)) {
            if (!line.empty()) {
                try { v.push_back(Notification::deserialize(line)); } catch (...) {}
            }
        }
        f.close();
        return v;
    }

    // ---- SAVE ALL ----
    void saveAll(const vector<Student>& stu, const vector<Company>& com,
                 const vector<Internship>& intern, const vector<Application>& apps,
                 const vector<Notification>& notifs) {
        saveStudents(stu);
        saveCompanies(com);
        saveInternships(intern);
        saveApplications(apps);
        saveNotifications(notifs);
    }
};

// ======================================================================
//  RESUME SCORER - Intelligent scoring algorithm
// ======================================================================
class ResumeScorer {
public:
    // Score formula: CGPA(30%) + SkillMatch(50%) + ProfileCompleteness(20%)
    static int calculateScore(const Student& student, const Internship& internship) {
        // CGPA component: normalize to 0-30
        float cgpaScore = (student.getCgpa() / 10.0f) * 30.0f;

        // Skill match component: 0-50
        float skillScore = 0;
        vector<string> reqSkills = internship.getRequiredSkills();
        vector<string> stuSkills = student.getSkills();
        int matched = countMatchedSkills(stuSkills, reqSkills);
        if (!reqSkills.empty()) {
            skillScore = ((float)matched / (float)reqSkills.size()) * 50.0f;
        }

        // Profile completeness: 0-20
        float profileScore = student.getProfileCompleteness() * 0.2f;

        return (int)round(cgpaScore + skillScore + profileScore);
    }

    static int calculateMatchPercentage(const Student& student, const Internship& internship) {
        vector<string> reqSkills = internship.getRequiredSkills();
        vector<string> stuSkills = student.getSkills();
        if (reqSkills.empty()) return 100;
        int matched = countMatchedSkills(stuSkills, reqSkills);
        return (int)round(((float)matched / (float)reqSkills.size()) * 100.0f);
    }

    static int countMatchedSkills(const vector<string>& stuSkills, const vector<string>& reqSkills) {
        int count = 0;
        for (auto& req : reqSkills) {
            for (auto& stu : stuSkills) {
                if (StrUtil::toLower(StrUtil::trim(stu)) == StrUtil::toLower(StrUtil::trim(req))) {
                    count++;
                    break;
                }
            }
        }
        return count;
    }

    static vector<string> getMissingSkills(const Student& student, const Internship& internship) {
        vector<string> missing;
        for (auto& req : internship.getRequiredSkills()) {
            bool found = false;
            for (auto& stu : student.getSkills()) {
                if (StrUtil::toLower(StrUtil::trim(stu)) == StrUtil::toLower(StrUtil::trim(req))) {
                    found = true;
                    break;
                }
            }
            if (!found) missing.push_back(req);
        }
        return missing;
    }
};

// ======================================================================
//  MATCH ENGINE - Recommendations & Search
// ======================================================================

// Template function for generic filtering (demonstrates TEMPLATES)
template <typename T, typename Pred>
vector<T> filterItems(const vector<T>& items, Pred predicate) {
    vector<T> result;
    copy_if(items.begin(), items.end(), back_inserter(result), predicate);
    return result;
}

class MatchEngine {
public:
    // Recommend internships for a student based on skills + CGPA
    static vector<pair<int, Internship>> recommendForStudent(
        const Student& student, const vector<Internship>& internships,
        const vector<Application>& apps) {

        // Get already-applied internship IDs
        set<int> appliedIDs;
        for (auto& a : apps) {
            if (a.getStudentID() == student.getId()) appliedIDs.insert(a.getInternshipID());
        }

        vector<pair<int, Internship>> scored; // (compatibility score, internship)
        for (auto& intern : internships) {
            if (!intern.isOpen()) continue;
            if (appliedIDs.count(intern.getId())) continue;

            int matchPct = ResumeScorer::calculateMatchPercentage(student, intern);
            int score = ResumeScorer::calculateScore(student, intern);
            int compatibility = (matchPct + score) / 2;
            scored.push_back({compatibility, intern});
        }

        // Sort by compatibility descending
        sort(scored.begin(), scored.end(), [](auto& a, auto& b) { return a.first > b.first; });
        return scored;
    }

    // Recommend students for a company's internship
    static vector<pair<int, Student>> recommendForInternship(
        const Internship& internship, const vector<Student>& students,
        const vector<Application>& apps) {

        set<int> appliedStudentIDs;
        for (auto& a : apps) {
            if (a.getInternshipID() == internship.getId())
                appliedStudentIDs.insert(a.getStudentID());
        }

        vector<pair<int, Student>> scored;
        for (auto& stu : students) {
            if (appliedStudentIDs.count(stu.getId())) continue;
            int score = ResumeScorer::calculateScore(stu, internship);
            if (score > 20) scored.push_back({score, stu});
        }

        sort(scored.begin(), scored.end(), [](auto& a, auto& b) { return a.first > b.first; });
        return scored;
    }

    // Auto-shortlist: top 30% by resume score
    static vector<int> autoShortlist(vector<Application>& apps, int internshipID) {
        vector<Application*> forIntern;
        for (auto& a : apps) {
            if (a.getInternshipID() == internshipID && a.getStatus() == "APPLIED")
                forIntern.push_back(&a);
        }

        sort(forIntern.begin(), forIntern.end(),
             [](auto a, auto b) { return a->getResumeScore() > b->getResumeScore(); });

        int topCount = max(1, (int)(forIntern.size() * 0.3));
        vector<int> shortlisted;
        for (int i = 0; i < topCount && i < (int)forIntern.size(); i++) {
            forIntern[i]->setStatus("SHORTLISTED");
            shortlisted.push_back(forIntern[i]->getStudentID());
        }
        return shortlisted;
    }
};
