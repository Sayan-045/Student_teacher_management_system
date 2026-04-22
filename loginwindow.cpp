#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include "studentdashboard.h"
#include "teacherdashboard.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_loginButton_clicked()
{
    QString role = ui->comboBox->currentText();
    QString id = ui->idEdit->text();
    QString password = ui->passwordEdit->text();

    if (id.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter both ID and Password!");
        return;
    }

    if (role == "Student") {
        std::string sid = id.toStdString();
        std::string pass = password.toStdString();

        if (!db.userExists("students", sid)) {
            // New Student Registration
            bool ok;
            QString name = QInputDialog::getText(this, "Signup", "Enter your name:", QLineEdit::Normal, "", &ok);
            if (!ok || name.isEmpty()) return;

            db.insertUser("students", sid, name.toStdString(), pass);
            QMessageBox::information(this, "Success", "Account created successfully!");
        } else {
            // Existing Student Login
            if (!db.checkPassword("students", "id", sid, pass)) {
                QMessageBox::critical(this, "Error", "Incorrect Password!");
                return;
            }
        }

        StudentDashboard *sd = new StudentDashboard(sid, &db);
        sd->show();
        this->close();
    }
    else { // Teacher Role
        std::string courseId = id.toStdString();
        std::string pass = password.toStdString();

        if (!db.courseExists(courseId)) {
            // New Course Registration
            db.addCourse(courseId, 30, pass);
            QMessageBox::information(this, "Success", "New Course created successfully!");
        } else {
            // Existing Course Login
            if (!db.checkPassword("courses", "course_id", courseId, pass)) {
                QMessageBox::critical(this, "Error", "Incorrect Course Password!");
                return;
            }
        }

        TeacherDashboard *t = new TeacherDashboard(courseId, &db);
        t->show();
        this->close();
    }
}
void LoginWindow::on_resetButton_clicked()
{
    QString role = ui->comboBox->currentText();
    QString id = ui->idEdit->text();

    if (id.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter your ID/Course Code first to reset the password.");
        return;
    }

    std::string targetId = id.toStdString();
    std::string table = (role == "Student") ? "students" : "courses";
    std::string idCol = (role == "Student") ? "id" : "course_id";

    // Check if user/course actually exists before resetting
    bool exists = (role == "Student") ? db.userExists(table, targetId) : db.courseExists(targetId);

    if (!exists) {
        QMessageBox::warning(this, "Error", "Account/Course not found!");
        return;
    }

    bool ok;
    QString newPassword = QInputDialog::getText(this, "Reset Password",
                                                "Enter new password:",
                                                QLineEdit::Password,
                                                "", &ok);

    if (ok && !newPassword.isEmpty()) {
        db.updatePassword(table, idCol, targetId, newPassword.toStdString());
        QMessageBox::information(this, "Success", "Password updated successfully!");
    }
}