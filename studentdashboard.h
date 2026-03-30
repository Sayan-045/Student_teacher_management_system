#ifndef STUDENTDASHBOARD_H
#define STUDENTDASHBOARD_H

#include <QMainWindow>
#include <string>
#include "Database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class StudentDashboard; }
QT_END_NAMESPACE

class StudentDashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit StudentDashboard(std::string id, Database* db, QWidget *parent = nullptr);
    ~StudentDashboard();

private slots:
    void on_dayWiseBtn_clicked();
    void on_addCourseBtn_clicked();
    void on_viewAttendanceBtn_clicked();
    void on_reportBtn_clicked();
    void on_logoutBtn_clicked();

private:
    Ui::StudentDashboard *ui;
    std::string student_id;
    Database* db;
};

#endif // STUDENTDASHBOARD_H