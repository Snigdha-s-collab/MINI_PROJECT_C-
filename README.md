# SIAMS — Smart Internship Application Management System

A comprehensive console-based C++ application that manages internship applications between **Students** and **Companies**, featuring intelligent scoring algorithms, smart matching, and real-time analytics.

## 🎯 Problem Statement

Managing internship applications manually is inefficient for both students and companies. Students struggle to find relevant opportunities, while companies face challenges in screening large volumes of applicants. **SIAMS** automates this process with intelligent matching and scoring algorithms.

## 🏗️ Architecture & OOP Concepts

| OOP Concept | Implementation |
|---|---|
| **Inheritance** | `User` (base) → `Student`, `Company` (derived classes) |
| **Polymorphism** | Virtual `getRole()`, `displayDashboardHeader()` methods |
| **Encapsulation** | Private members with getters/setters in all entities |
| **Abstraction** | `ResumeScorer`, `MatchEngine`, `FileManager` abstract complex logic |
| **Operator Overloading** | `<<` for display, `==` for comparison |
| **Templates** | Generic `filterItems<T>()` for search/filter operations |
| **STL Containers** | `vector`, `map`, `set`, `queue`, `pair` throughout |
| **File I/O** | `fstream`-based persistence layer |
| **Exception Handling** | Try-catch for file operations and input validation |
| **Friend Functions** | Serialization via `operator<<` |

## ✨ Features

### Core Features
- 👤 **Student & Company Registration/Login** with hashed passwords (djb2)
- 📝 **Post Internships** (company) / **Browse & Apply** (student)
- 📊 **Dashboard Analytics** — application stats, success rates, skill insights
- 🔐 **Change Password** — secure password update with confirmation
- 📄 **Export Reports** — export profile and application history to text files

### Innovative Algorithms
- 🧠 **Intelligent Resume Scoring** — CGPA (30%) + Skill Match (50%) + Profile Completeness (20%)
- 🔗 **Smart Skill Matching** — case-insensitive matching with missing skill suggestions
- ⚡ **Auto-Shortlisting** — top 30% applicants automatically shortlisted
- 🎯 **Recommendation Engine** — suggests internships/candidates based on compatibility

### Management Features
- 📅 **Deadline Management** — auto-close expired internships, days-remaining counter
- ⭐ **Company Rating System** — 1-5 star ratings from students
- 🚫 **Conflict Detection** — duplicate applications, CGPA warnings, selection conflicts
- 🔍 **Advanced Search & Filter** — by stipend, location, skills, CGPA, rating
- 🔎 **Keyword Search** — search internships by role, company name, or skill keywords
- 🔔 **Notification System** — real-time notifications with unread counter
- 📋 **Application Details** — detailed view of any application with full scoring breakdown
- ✏️ **Profile Editing** — update name, email, CGPA, skills, resume, industry
- 🛡️ **Admin Panel** — system-wide statistics and data management

## 🛠️ Technology

- **Language:** C++17
- **Build:** g++ (MinGW / GCC)
- **Storage:** File-based (pipe-delimited `.dat` files)
- **UI:** ANSI color-coded console with box-drawing characters
- **Dependencies:** None (pure C++ standard library)

## 📁 Project Structure

```
MINI_PROJECT_C-/
├── src/
│   ├── main.cpp          # Application entry point & SIAMS class
│   ├── Utils.h           # Colors, date, hashing, input utilities
│   ├── Models.h          # Entity classes (User, Student, Company, etc.)
│   └── Engine.h          # FileManager, ResumeScorer, MatchEngine
├── data/                 # Auto-created runtime data files
├── Makefile              # Build configuration
├── .gitignore
└── README.md
```

## 🚀 Build & Run

### Prerequisites
- g++ compiler with C++17 support
- Windows 10+ terminal (for ANSI color support)

### Compile & Run
```bash
# Using g++ directly
g++ -std=c++17 -Wall -o siams.exe src/main.cpp
./siams.exe

# Using Make
make
make run
```

### Demo Credentials
- **Admin Panel:** Password = `admin123`
- **Students/Companies:** Register new accounts through the application

## 📊 Resume Scoring Formula

```
resumeScore = (CGPA/10 × 30) + (matchedSkills/requiredSkills × 50) + (profileCompleteness × 0.2)
```

| Component | Weight | Description |
|---|---|---|
| CGPA | 30% | Normalized to 0-30 scale |
| Skill Match | 50% | Matched skills / Required skills × 50 |
| Profile | 20% | Based on filled fields (name, email, CGPA, skills, resume) |

## 📋 User Workflows

### Student Workflow
1. **Register** → Create account with name, email, CGPA, skills, resume
2. **Browse/Search** → Find internships with filters (stipend, location, skills)
3. **Apply** → Submit application with auto-calculated resume score & match %
4. **Track** → Monitor application status via dashboard & analytics
5. **Rate** → Rate companies after interview process
6. **Export** → Generate text report of profile and application history

### Company Workflow
1. **Register** → Create company account with industry details
2. **Post** → Create internship listings with requirements
3. **Review** → View applicants with scores, auto-shortlist top 30%
4. **Manage** → Update application statuses (shortlist/reject/select)
5. **Recommend** → View suggested candidates who haven't applied yet
6. **Export** → Generate text report of internships and applications

## 👥 Team

**Mini Project — Object-Oriented Programming (C++)**

---

*Built with ❤️ using C++17*