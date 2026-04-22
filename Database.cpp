#include "Database.h"
#include <iostream>
#include <QInputDialog>
#include <QString>

Database::Database() {
    sqlite3_open("portal.db", &DB);
    execute("CREATE TABLE IF NOT EXISTS students(id TEXT PRIMARY KEY, name TEXT, password TEXT);");
    execute("CREATE TABLE IF NOT EXISTS courses(course_id TEXT PRIMARY KEY, total_classes INT, password TEXT);");
    execute("CREATE TABLE IF NOT EXISTS enrollments(student_id TEXT, course_id TEXT);");
    execute("CREATE TABLE IF NOT EXISTS attendance(student_id TEXT, course_id TEXT, day INT, present INT);");
    execute("CREATE TABLE IF NOT EXISTS marks(student_id TEXT, course_id TEXT, marks INT);");
}

Database::~Database() {
    sqlite3_close(DB);
}

bool Database::execute(const std::string& sql) {
    char* errMsg;
    if (sqlite3_exec(DB, sql.c_str(), NULL, NULL, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::userExists(const std::string& table, const std::string& id) {
    std::string sql = "SELECT * FROM " + table + " WHERE id='" + id + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

void Database::insertUser(const std::string& table, const std::string& id, const std::string& name, const std::string& password) {
    std::string sql = "INSERT INTO " + table + " VALUES('" + id + "','" + name + "','" + password + "');";
    execute(sql);
}

bool Database::courseExists(const std::string& course_id) {
    std::string sql = "SELECT * FROM courses WHERE course_id='" + course_id + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

void Database::addCourse(const std::string& course_id, int total_classes, const std::string& password) {
    if (courseExists(course_id)) return;
    std::string sql = "INSERT INTO courses VALUES('" + course_id + "'," + std::to_string(total_classes) + ",'" + password + "');";
    execute(sql);
}

bool Database::checkPassword(const std::string& table, const std::string& idCol, const std::string& id, const std::string& password) {
    std::string sql = "SELECT * FROM " + table + " WHERE " + idCol + "='" + id + "' AND password='" + password + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    bool valid = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return valid;
}

void Database::updatePassword(const std::string& table, const std::string& idCol, const std::string& id, const std::string& newPassword) {
    std::string sql = "UPDATE " + table + " SET password='" + newPassword + "' WHERE " + idCol + "='" + id + "';";
    execute(sql);
}

bool Database::enrollStudent(const std::string& student_id, const std::string& course_id) {
    if (!courseExists(course_id)) return false;

    if (isEnrolled(student_id, course_id)) {
        return false;
    }

    execute("INSERT INTO enrollments VALUES('" + student_id + "','" + course_id + "');");
    return true;
}

bool Database::isEnrolled(const std::string& student_id, const std::string& course_id) {
    std::string sql = "SELECT * FROM enrollments WHERE student_id='" + student_id + "' AND course_id='" + course_id + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

bool Database::markAttendance(const std::string& course_id, int day) {
    std::string sql = "SELECT student_id FROM enrollments WHERE course_id='" + course_id + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    bool success = false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string sid = (const char*)sqlite3_column_text(stmt, 0);

        bool ok;
        QString prompt = QString("Student %1\nEnter 1 for Present, 0 for Absent:").arg(sid.c_str());

        int p = QInputDialog::getInt(nullptr, "Mark Attendance", prompt, 1, 0, 1, 1, &ok);

        if (!ok) {
            sqlite3_finalize(stmt);
            return false;
        }

        execute("INSERT INTO attendance VALUES('" + sid + "','" + course_id + "'," +
                std::to_string(day) + "," + std::to_string(p) + ");");
        success = true;
    }
    sqlite3_finalize(stmt);
    return success;
}


bool Database::addMarks(const std::string& course_id) {
    std::string sql = "SELECT student_id FROM enrollments WHERE course_id='" + course_id + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    bool success = false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string sid = (const char*)sqlite3_column_text(stmt, 0);

        bool ok;
        QString prompt = QString("Enter Marks (0-100) for student %1:").arg(sid.c_str());

        int m = QInputDialog::getInt(nullptr, "Add Marks", prompt, 0, 0, 100, 1, &ok);

        if (!ok) {
            sqlite3_finalize(stmt);
            return false;
        }

        execute("INSERT OR REPLACE INTO marks VALUES('" + sid + "','" + course_id + "'," +
                std::to_string(m) + ");");
        success = true;
    }
    sqlite3_finalize(stmt);
    return success;
}

int Database::getCurrentDay(const std::string& course_id) {
    std::string sql = "SELECT MAX(day) FROM attendance WHERE course_id='" + course_id + "';";
    sqlite3_stmt* stmt;
    int max_day = 0;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            max_day = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return max_day;
}

bool Database::updateSpecificAttendance(const std::string& course_id, int day, const std::string& student_id, int present) {
    std::string sqlCheck = "SELECT * FROM attendance WHERE course_id='" + course_id + "' AND student_id='" + student_id + "' AND day=" + std::to_string(day) + ";";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sqlCheck.c_str(), -1, &stmt, NULL);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);

    if (!exists) return false;

    // Update the record
    std::string sqlUpdate = "UPDATE attendance SET present=" + std::to_string(present) +
                            " WHERE course_id='" + course_id + "' AND student_id='" + student_id + "' AND day=" + std::to_string(day) + ";";
    return execute(sqlUpdate);
}

std::string Database::viewStudentsInCourse(const std::string& course_id) {
    std::string sql = "SELECT s.id, s.name FROM students s JOIN enrollments e ON s.id = e.student_id WHERE e.course_id='" + course_id + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    std::string result = "--- Students in Course ---\n\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        result += "ID: " + std::string((const char*)sqlite3_column_text(stmt, 0)) +
                  "  |  Name: " + std::string((const char*)sqlite3_column_text(stmt, 1)) + "\n";
    }
    sqlite3_finalize(stmt);
    if (!found) return "No students enrolled yet.";
    return result;
}

std::string Database::viewStudentAttendance(const std::string& student_id) {
    std::string sql = "SELECT c.course_id, c.total_classes, COUNT(a.present), SUM(a.present) FROM courses c LEFT JOIN attendance a ON c.course_id = a.course_id AND a.student_id='" + student_id + "' JOIN enrollments e ON c.course_id = e.course_id WHERE e.student_id='" + student_id + "' GROUP BY c.course_id;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    std::string result = "--- Attendance Record ---\n\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        std::string course = (const char*)sqlite3_column_text(stmt, 0);
        int total_classes = sqlite3_column_int(stmt, 1);
        int total_taken = sqlite3_column_int(stmt, 2);
        int present = sqlite3_column_int(stmt, 3);

        float percent = (total_taken == 0) ? 0 : (present * 100.0 / total_taken);

        int needed = 0;
        while (true) {
            float new_percent = ((present + needed) * 100.0) / (total_taken + needed);
            if (new_percent >= 75.0 || (total_taken + needed) >= total_classes) break;
            needed++;
        }

        result += "Course: " + course + "\nAttendance: " + std::to_string((int)percent) + "%\nNeed " + std::to_string(needed) + " more classes for 75%\n\n";
    }
    sqlite3_finalize(stmt);
    if(!found) return "No attendance records found.";
    return result;
}

std::string Database::generateReportCard(const std::string& student_id) {
    std::string sql = "SELECT e.course_id, COUNT(a.present), SUM(a.present), m.marks FROM enrollments e LEFT JOIN attendance a ON e.student_id=a.student_id AND e.course_id=a.course_id LEFT JOIN marks m ON e.student_id=m.student_id AND e.course_id=m.course_id WHERE e.student_id='" + student_id + "' GROUP BY e.course_id;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    std::string result = "--- Official Report Card ---\n\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        std::string c = (const char*)sqlite3_column_text(stmt, 0);
        int total = sqlite3_column_int(stmt, 1);
        int present = sqlite3_column_int(stmt, 2);
        int marks = sqlite3_column_int(stmt, 3);

        float percent = (total == 0) ? 0 : present * 100.0 / total;

        std::string grade = "D";
        if (marks >= 90) grade = "A";
        else if (marks >= 75) grade = "B";
        else if (marks >= 60) grade = "C";

        result += c + " | Att: " + std::to_string((int)percent) + "% | Marks: " + std::to_string(marks) + " | Grade: " + grade + "\n";
    }
    sqlite3_finalize(stmt);
    if(!found) return "No records found.";
    return result;
}

std::string Database::viewCourseReport(const std::string& course_id) {
    std::string sql = "SELECT s.id, s.name, COUNT(a.present), SUM(a.present), m.marks FROM enrollments e JOIN students s ON e.student_id = s.id LEFT JOIN attendance a ON e.student_id = a.student_id AND e.course_id = a.course_id LEFT JOIN marks m ON e.student_id = m.student_id AND e.course_id = m.course_id WHERE e.course_id='" + course_id + "' GROUP BY s.id;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    std::string result = "--- Master Course Report ---\n\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        std::string id = (const char*)sqlite3_column_text(stmt, 0);
        std::string name = (const char*)sqlite3_column_text(stmt, 1);

        int total = sqlite3_column_int(stmt, 2);
        int present = sqlite3_column_int(stmt, 3);
        int marks = sqlite3_column_int(stmt, 4);

        float percent = (total == 0) ? 0 : present * 100.0 / total;

        std::string grade = "D";
        if (marks >= 90) grade = "A";
        else if (marks >= 75) grade = "B";
        else if (marks >= 60) grade = "C";

        result += "ID: " + id + " | " + name + " | Att: " + std::to_string((int)percent) + "% | Marks: " + std::to_string(marks) + " | Grade: " + grade + "\n";
    }
    sqlite3_finalize(stmt);
    if(!found) return "No records found for this course.";
    return result;
}

// --- NEW ADD ONS ---

QStringList Database::getAvailableCourses() {
    QStringList list;
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, "SELECT course_id FROM courses;", -1, &stmt, NULL);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list << (const char*)sqlite3_column_text(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return list;
}

QStringList Database::getEnrolledCourses(const std::string& student_id) {
    QStringList list;
    std::string sql = "SELECT course_id FROM enrollments WHERE student_id='" + student_id + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list << (const char*)sqlite3_column_text(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return list;
}

QString Database::getDayWiseAttendance(const std::string& student_id, const std::string& course_id) {
    std::string sql = "SELECT day, present FROM attendance WHERE student_id='" + student_id + "' AND course_id='" + course_id + "' ORDER BY day ASC;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    QString res = "--- Day-wise Record ---\n\n";
    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        int day = sqlite3_column_int(stmt, 0);
        int present = sqlite3_column_int(stmt, 1);
        res += "Day " + QString::number(day) + ": " + (present ? "✅ Present" : "❌ Absent") + "\n";
    }
    sqlite3_finalize(stmt);
    return found ? res : "No attendance marked yet.";
}

std::vector<std::pair<QString, QString>> Database::getStudentsInCourseData(const std::string& course_id) {
    std::vector<std::pair<QString, QString>> data;
    std::string sql = "SELECT s.id, s.name FROM students s JOIN enrollments e ON s.id = e.student_id WHERE e.course_id='" + course_id + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        data.push_back({(const char*)sqlite3_column_text(stmt, 0), (const char*)sqlite3_column_text(stmt, 1)});
    }
    sqlite3_finalize(stmt);
    return data;
}

std::vector<ReportRow> Database::getCourseReportData(const std::string& course_id) {
    std::vector<ReportRow> data;
    std::string sql = "SELECT s.id, s.name, COUNT(a.present), SUM(a.present), m.marks FROM enrollments e JOIN students s ON e.student_id = s.id LEFT JOIN attendance a ON e.student_id = a.student_id AND e.course_id = a.course_id LEFT JOIN marks m ON e.student_id = m.student_id AND e.course_id = m.course_id WHERE e.course_id='" + course_id + "' GROUP BY s.id;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ReportRow row;
        row.id = (const char*)sqlite3_column_text(stmt, 0);
        row.name = (const char*)sqlite3_column_text(stmt, 1);
        int total = sqlite3_column_int(stmt, 2);
        int present = sqlite3_column_int(stmt, 3);
        row.marks = sqlite3_column_int(stmt, 4);

        row.attPercent = (total == 0) ? 0 : (present * 100) / total;

        row.grade = "D";
        if (row.marks >= 90) row.grade = "A";
        else if (row.marks >= 75) row.grade = "B";
        else if (row.marks >= 60) row.grade = "C";

        data.push_back(row);
    }
    sqlite3_finalize(stmt);
    return data;
}