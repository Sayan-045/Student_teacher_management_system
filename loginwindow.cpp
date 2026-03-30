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

    if (id.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter ID!");
        return;
    }

    if (role == "Student") {
        std::string sid = id.toStdString();

        if (!db.userExists("students", sid)) {
            bool ok;
            QString name = QInputDialog::getText(this, "Signup",
                                                 "Enter your name:",
                                                 QLineEdit::Normal,
                                                 "", &ok);
            if (!ok || name.isEmpty()) return;

            db.insertUser("students", sid, name.toStdString());
            QMessageBox::information(this, "Success", "Account created!");
        }

        StudentDashboard *sd = new StudentDashboard(sid, &db);
        sd->show();
        this->close();
    }
    else {
        std::string courseId = id.toStdString();

        if (!db.courseExists(courseId)) {
            db.addCourse(courseId, 30);
        }

        TeacherDashboard *t = new TeacherDashboard(courseId, &db);
        t->show();
        this->close();
    }
}