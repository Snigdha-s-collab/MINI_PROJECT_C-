#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

string hashPassword(const string& pwd) {
    unsigned long hash = 5381;
    for (char c : pwd) hash = ((hash << 5) + hash) + c;
    stringstream ss;
    ss << hex << hash;
    return ss.str();
}

int main() {
    string dataDir = "data";
    
    string pass123 = hashPassword("pass123");
    string tech123 = hashPassword("tech123");
    string fin123 = hashPassword("fin123");
    string cloud123 = hashPassword("cloud123");
    string green123 = hashPassword("green123");
    string data123 = hashPassword("data123");
    string steel123 = hashPassword("steel123");
    string pixel123 = hashPassword("pixel123");
    string quantum123 = hashPassword("quantum123");
    string swift123 = hashPassword("swift123");
    string medi123 = hashPassword("medi123");

    // STUDENTS
    {
        ofstream f(dataDir + "/students.dat", ios::trunc);
        f << "1|Aarav Sharma|aarav@gmail.com|" << pass123 << "|8.5|C++,Python,DSA,ML|CSE student with strong coding skills and ML experience. Completed projects in image classification and web scraping.|1,5,10\n";
        f << "2|Priya Patel|priya.p@gmail.com|" << pass123 << "|9|Python,SQL,Excel,Statistics,PowerBI|Data-driven BTech student passionate about analytics and business intelligence. Interned at a startup for data analysis.|2,6\n";
        f << "3|Rohan Gupta|rohan.g@gmail.com|" << pass123 << "|7.8|HTML,CSS,JavaScript,React,Node.js|Full-stack web developer with 3 personal projects including an e-commerce site and a blog platform.|3,9\n";
        f << "4|Sneha Reddy|sneha.r@gmail.com|" << pass123 << "|8.2|Python,Pandas,TensorFlow,ML,Deep Learning|AI enthusiast with published paper on NLP sentiment analysis. Strong mathematical foundation.|4,7\n";
        f << "5|Vikram Singh|vikram.s@gmail.com|" << pass123 << "|7.5|AWS,Docker,Linux,Git,CI/CD|Cloud computing enthusiast with AWS certification. Built and deployed microservices architecture.|8,11\n";
        f << "6|Ananya Krishnan|ananya.k@gmail.com|" << pass123 << "|9.2|C++,Java,DSA,Python,SQL|Competitive programmer with 5-star rating on CodeChef. Strong in algorithms and system design.|12,13\n";
        f << "7|Arjun Mehta|arjun.m@gmail.com|" << pass123 << "|7.2|Figma,Adobe XD,Photoshop,CSS,HTML|Creative designer with portfolio of 10+ UI/UX projects. Passionate about user-centered design.|14\n";
        f << "8|Diya Nair|diya.n@gmail.com|" << pass123 << "|8.8|Python,PyTorch,NLP,Deep Learning,Statistics|Research-oriented student working on transformer models. Published work on text summarization using BERT.|15,16\n";
        f << "9|Karthik Iyer|karthik.i@gmail.com|" << pass123 << "|6.8|AutoCAD,Excel,Manufacturing,SolidWorks|Mechanical engineering student with hands-on workshop experience and CAD modeling expertise.|17\n";
        f << "10|Meera Joshi|meera.j@gmail.com|" << pass123 << "|8|Excel,SQL,Python,Communication,PowerBI|Commerce background with technical skills. Completed Google Data Analytics certification.|18,19\n";
        f << "11|Rahul Verma|rahul.v@gmail.com|" << pass123 << "|7.6|Python,Django,SQL,Git,Docker|Backend developer with experience in building REST APIs. Contributed to 2 open-source projects.|20,21\n";
        f << "12|Ishita Banerjee|ishita.b@gmail.com|" << pass123 << "|8.9|Python,ML,Pandas,SQL,Tableau|Data science aspirant with Kaggle competitions experience. Built predictive models for healthcare data.|22,23\n";
        f << "13|Aditya Kulkarni|aditya.k@gmail.com|" << pass123 << "|7|JavaScript,React,Node.js,MongoDB,CSS|MERN stack developer who built a real-time chat application and a task management tool.|24\n";
        f << "14|Nisha Agarwal|nisha.a@gmail.com|" << pass123 << "|8.3|C++,Python,DSA,Kubernetes,Linux|Systems programming enthusiast with experience in OS development and container orchestration.|25,26\n";
        f << "15|Siddharth Rao|siddharth.r@gmail.com|" << pass123 << "|7.9|Python,AWS,SQL,Excel,Git|Versatile engineer with cloud and data skills. Completed internship at a fintech startup.|27,28\n";
        f.close();
    }

    // COMPANIES
    {
        ofstream f(dataDir + "/companies.dat", ios::trunc);
        f << "1|TechNova Solutions|hr@technova.com|" << tech123 << "|IT|4.200000|5|21.000000|1,2|3\n";
        f << "2|FinVault Capital|careers@finvault.com|" << fin123 << "|Finance|3.800000|4|15.200000|3|2\n";
        f << "3|CloudMatrix Inc|jobs@cloudmatrix.io|" << cloud123 << "|IT|4.500000|6|27.000000|4,5|4\n";
        f << "4|GreenLeaf Consulting|apply@greenleaf.com|" << green123 << "|Consulting|3.500000|4|14.000000|6|1\n";
        f << "5|DataForge Analytics|recruit@dataforge.in|" << data123 << "|IT|4.700000|3|14.100000|7|2\n";
        f << "6|SteelBridge Manufacturing|hr@steelbridge.com|" << steel123 << "|Manufacturing|3.200000|5|16.000000|8|1\n";
        f << "7|PixelCraft Studios|talent@pixelcraft.com|" << pixel123 << "|IT|4.000000|4|16.000000|9|2\n";
        f << "8|QuantumLeap AI|hiring@quantumleap.ai|" << quantum123 << "|IT|4.800000|5|24.000000|10|3\n";
        f << "9|SwiftLogistics Corp|careers@swiftlog.com|" << swift123 << "|Other|3.600000|3|10.800000||0\n";
        f << "10|MediCare Health Tech|hr@medicare-ht.com|" << medi123 << "|IT|4.100000|4|16.400000||1\n";
        f.close();
    }

    // INTERNSHIPS
    {
        ofstream f(dataDir + "/internships.dat", ios::trunc);
        f << "1|1|Software Developer Intern|C++,Python,DSA|7.000000|25000|3|Bangalore|30-06-2026|50|8|OPEN\n";
        f << "2|1|Web Development Intern|HTML,CSS,JavaScript,React|6.500000|20000|2|Hyderabad|15-07-2026|30|5|OPEN\n";
        f << "3|2|Financial Analyst Intern|Excel,SQL,Python,Statistics|8.000000|30000|6|Mumbai|20-05-2026|20|6|OPEN\n";
        f << "4|3|Cloud Engineering Intern|AWS,Docker,Linux,Python|7.500000|35000|3|Pune|25-06-2026|25|7|OPEN\n";
        f << "5|3|DevOps Intern|Docker,Kubernetes,CI/CD,Git|7.000000|28000|4|Remote|10-08-2026|15|3|OPEN\n";
        f << "6|4|Business Analyst Intern|Excel,PowerBI,SQL,Communication|7.500000|22000|3|Delhi|01-06-2026|20|4|OPEN\n";
        f << "7|5|Data Science Intern|Python,ML,Pandas,TensorFlow|8.000000|40000|6|Bangalore|15-06-2026|10|5|OPEN\n";
        f << "8|6|Industrial Engineering Intern|AutoCAD,Excel,Manufacturing|6.000000|15000|2|Chennai|30-07-2026|25|2|OPEN\n";
        f << "9|7|UI/UX Design Intern|Figma,Adobe XD,Photoshop,CSS|6.500000|18000|3|Mumbai|20-06-2026|15|4|OPEN\n";
        f << "10|8|AI/ML Research Intern|Python,PyTorch,NLP,Deep Learning|8.500000|45000|6|Bangalore|01-09-2026|8|3|OPEN\n";
        f.close();
    }

    // APPLICATIONS
    {
        ofstream f(dataDir + "/applications.dat", ios::trunc);
        f << "1|1|1|10-04-2026|SHORTLISTED|78|75|Strong DSA skills\n";
        f << "2|2|3|08-04-2026|SELECTED|85|75|Excellent analytical skills\n";
        f << "3|3|2|09-04-2026|SHORTLISTED|82|75|Good frontend portfolio\n";
        f << "4|4|4|10-04-2026|APPLIED|58|25|\n";
        f << "5|1|5|12-04-2026|APPLIED|52|25|\n";
        f << "6|2|6|11-04-2026|SHORTLISTED|80|75|Great fit\n";
        f << "7|4|7|12-04-2026|SELECTED|88|75|Outstanding ML skills\n";
        f << "8|5|8|13-04-2026|REJECTED|35|0|Skills mismatch\n";
        f << "9|3|9|14-04-2026|APPLIED|55|25|\n";
        f << "10|1|10|15-04-2026|APPLIED|65|50|\n";
        f << "11|5|5|14-04-2026|SELECTED|90|100|Perfect match for DevOps\n";
        f << "12|6|1|05-04-2026|SELECTED|92|66|Top performer in coding test\n";
        f << "13|6|3|07-04-2026|SHORTLISTED|72|50|\n";
        f << "14|7|9|11-04-2026|SHORTLISTED|80|75|Impressive portfolio\n";
        f << "15|8|10|06-04-2026|SELECTED|95|100|Exceptional research background\n";
        f << "16|8|7|08-04-2026|SHORTLISTED|82|50|\n";
        f << "17|9|8|15-04-2026|SELECTED|85|66|Strong hands-on skills\n";
        f << "18|10|6|09-04-2026|APPLIED|75|75|\n";
        f << "19|10|3|10-04-2026|APPLIED|70|50|\n";
        f << "20|11|1|07-04-2026|APPLIED|60|33|\n";
        f << "21|11|4|09-04-2026|SHORTLISTED|68|50|Docker experience noted\n";
        f << "22|12|7|06-04-2026|SHORTLISTED|80|50|Kaggle experience valued\n";
        f << "23|12|3|08-04-2026|APPLIED|72|50|\n";
        f << "24|13|2|12-04-2026|SELECTED|85|75|Full-stack proficiency\n";
        f << "25|14|1|08-04-2026|SHORTLISTED|82|66|Solid systems knowledge\n";
        f << "26|14|5|10-04-2026|APPLIED|65|25|\n";
        f << "27|15|4|11-04-2026|APPLIED|58|25|\n";
        f << "28|15|3|13-04-2026|APPLIED|62|50|\n";
        f.close();
    }

    // NOTIFICATIONS
    {
        ofstream f(dataDir + "/notifications.dat", ios::trunc);
        f << "1|1|student|New internship matching your skills: Software Developer Intern at TechNova Solutions|18-04-2026 10:30|1\n";
        f << "2|1|student|Your application for Software Developer Intern has been SHORTLISTED!|15-04-2026 14:20|0\n";
        f << "3|2|student|Your application for Financial Analyst Intern: SELECTED|12-04-2026 09:00|1\n";
        f << "4|2|student|Your application for Business Analyst Intern has been SHORTLISTED!|13-04-2026 11:45|0\n";
        f << "5|3|student|Your application for Web Development Intern has been SHORTLISTED!|14-04-2026 16:00|0\n";
        f << "6|4|student|Your application for Data Science Intern: SELECTED|14-04-2026 10:30|1\n";
        f << "7|5|student|Your application for DevOps Intern: SELECTED|16-04-2026 09:15|1\n";
        f << "8|5|student|Application for Industrial Engineering Intern: REJECTED|15-04-2026 11:00|1\n";
        f << "9|6|student|Your application for Software Developer Intern: SELECTED|10-04-2026 15:30|1\n";
        f << "10|7|student|Your application for UI/UX Design Intern has been SHORTLISTED!|15-04-2026 14:00|0\n";
        f << "11|8|student|Your application for AI/ML Research Intern: SELECTED|12-04-2026 10:00|1\n";
        f << "12|9|student|Your application for Industrial Engineering Intern: SELECTED|17-04-2026 09:30|1\n";
        f << "13|11|student|Your application for Cloud Engineering Intern has been SHORTLISTED!|14-04-2026 13:00|0\n";
        f << "14|12|student|Your application for Data Science Intern has been SHORTLISTED!|13-04-2026 15:30|0\n";
        f << "15|13|student|Your application for Web Development Intern: SELECTED|16-04-2026 10:00|1\n";
        f << "16|14|student|Your application for Software Developer Intern has been SHORTLISTED!|13-04-2026 12:00|0\n";
        f << "17|1|company|New application for Software Developer Intern from Aarav Sharma|10-04-2026 09:00|1\n";
        f << "18|1|company|New application for Web Development Intern from Rohan Gupta|09-04-2026 10:30|1\n";
        f << "19|2|company|New application for Financial Analyst Intern from Priya Patel|08-04-2026 11:00|1\n";
        f << "20|3|company|New application for Cloud Engineering Intern from Sneha Reddy|10-04-2026 14:00|1\n";
        f << "21|3|company|New application for DevOps Intern from Vikram Singh|14-04-2026 09:30|1\n";
        f << "22|5|company|New application for Data Science Intern from Sneha Reddy|12-04-2026 10:00|1\n";
        f << "23|5|company|New application for Data Science Intern from Ishita Banerjee|06-04-2026 15:00|1\n";
        f << "24|8|company|New application for AI/ML Research Intern from Diya Nair|06-04-2026 08:30|1\n";
        f << "25|8|company|New application for AI/ML Research Intern from Aarav Sharma|15-04-2026 11:00|1\n";
        f << "26|7|company|New application for UI/UX Design Intern from Arjun Mehta|11-04-2026 09:45|1\n";
        f << "27|6|company|New application for Industrial Engineering Intern from Karthik Iyer|15-04-2026 10:15|1\n";
        f << "28|4|company|New application for Business Analyst Intern from Priya Patel|11-04-2026 13:30|1\n";
        f << "29|4|company|New application for Business Analyst Intern from Meera Joshi|09-04-2026 14:00|1\n";
        f << "30|10|student|New internship matching your skills: Business Analyst Intern at GreenLeaf Consulting|18-04-2026 11:00|0\n";
        f << "31|15|student|New internship matching your skills: Cloud Engineering Intern at CloudMatrix Inc|18-04-2026 11:30|0\n";
        f << "32|1|company|New application for Software Developer Intern from Ananya Krishnan|05-04-2026 12:00|1\n";
        f << "33|1|company|New application for Software Developer Intern from Nisha Agarwal|08-04-2026 10:30|1\n";
        f << "34|1|company|New application for Software Developer Intern from Rahul Verma|07-04-2026 09:00|1\n";
        f << "35|2|company|New application for Financial Analyst Intern from Meera Joshi|10-04-2026 11:30|1\n";
        f.close();
    }

    cout << "=== Data Generated Successfully ===" << endl;
    cout << "pass123 hash: " << pass123 << endl;
    return 0;
}
