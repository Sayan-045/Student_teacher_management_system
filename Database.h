#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"
#include <string>
#include <QStringList>
#include <vector>

struct ReportRow {
    QString id;
    QString name;
    int attPercent;
    int marks;
    QString grade;
};

class Database {
private:
    sqlite3* DB;

public:
    Database();
    ~Database();

    bool execute(const std::string& sql);

    bool userExists(const std::string& table, const std::string& id);
    void insertUser(const std::string& table, const std::string& id, const std::string& name);

    bool courseExists(const std::string& course_id);
    void addCourse(const std::string& course_id, int total_classes);

    void enrollStudent(const std::string& student_id, const std::string& course_id);

    void markAttendance(const std::string& course_id, int day);
    void addMarks(const std::string& course_id);

    QStringList getAvailableCourses();
    QStringList getEnrolledCourses(const std::string& student_id);
    QString getDayWiseAttendance(const std::string& student_id, const std::string& course_id);

    // For Teacher UI
    std::vector<std::pair<QString, QString>> getStudentsInCourseData(const std::string& course_id);
    std::vector<ReportRow> getCourseReportData(const std::string& course_id);
    // View functions now return strings to be displayed in the UI
    std::string viewStudentsInCourse(const std::string& course_id);
    std::string viewStudentAttendance(const std::string& student_id);
    std::string generateReportCard(const std::string& student_id);
    std::string viewCourseReport(const std::string& course_id);
};

#endif