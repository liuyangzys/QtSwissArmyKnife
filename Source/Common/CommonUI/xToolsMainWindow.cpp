﻿/***************************************************************************************************
 * Copyright 2023-2024 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded in "utf8 with bom", it is a part of xTools project.
 *
 * xTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source code
 * directory.
 **************************************************************************************************/
#include "xToolsMainWindow.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDesktopServices>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QLoggingCategory>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QStyle>
#include <QStyleFactory>
#include <QUrl>

#include "xToolsDataStructure.h"
#include "xToolsInterface.h"
#include "xToolsSettings.h"
#include "xToolsTranslator.h"

xToolsMainWindow::xToolsMainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_appStyleActionGroup = new QActionGroup(this);
    m_languageActionGroup = new QActionGroup(this);
    QString language = xToolsSettings::instance()->language();
    xToolsTranslator::instance()->setupLanguage(language);
    init();
}

xToolsMainWindow::~xToolsMainWindow() {}

void xToolsMainWindow::init()
{
    initMenu();
}

void xToolsMainWindow::initMenu()
{
    initMenuFile();
    initMenuOption();
    initMenuLanguage();
    initMenuHelp();
}

void xToolsMainWindow::initMenuFile()
{
    QMenuBar* menu_bar = menuBar();
    m_fileMenu = menu_bar->addMenu(tr("&File"));
    m_fileMenu->addAction(tr("&Exit"), this, &xToolsMainWindow::close);
}

void xToolsMainWindow::initMenuOption()
{
    m_optionMenu = new QMenu(tr("&Options"));
    menuBar()->addMenu(m_optionMenu);

    initOptionMenuAppStyleMenu();
    initOptionMenuSettingsMenu();
    initOptionMenuHdpiPolicy();
}

void xToolsMainWindow::initMenuLanguage()
{
    m_languageMenu = new QMenu(tr("&Languages"), this);
    menuBar()->addMenu(m_languageMenu);

    QStringList languages = xToolsTranslator::instance()->languanges();
    for (auto& language : languages) {
        QAction* action = new QAction(language, this);
        action->setCheckable(true);
        m_languageMenu->addAction(action);
        m_languageActionGroup->addAction(action);

        connect(action, &QAction::triggered, this, [=]() {
            xToolsSettings::instance()->setLanguage(language);
            tryToReboot();
        });

        QString setting_language = xToolsSettings::instance()->language();
        if (setting_language == language) {
            action->setChecked(true);
        }
    }
}

void xToolsMainWindow::initMenuHelp()
{
    QMenuBar* menu_bar = menuBar();
    m_helpMenu = menu_bar->addMenu(tr("&Help"));
    m_helpMenu->addAction(QIcon(":/resources/images/GitHub.png"),
                          "Github",
                          this,
                          &xToolsMainWindow::onGithubActionTriggered);
    m_helpMenu->addAction(QIcon(":/resources/images/Gitee.png"),
                          "Gitee",
                          this,
                          &xToolsMainWindow::onGiteeActionTriggered);
    m_helpMenu->addAction(QIcon(":/Resources/Icon/IconQQGray.svg"),
                          tr("User QQ Group"),
                          this,
                          &xToolsMainWindow::onUserQqGroupTriggerd);
    m_helpMenu->addAction(tr("&About"), this, &xToolsMainWindow::onAboutActionTriggered);
}

void xToolsMainWindow::initOptionMenuAppStyleMenu()
{
    QList<QAction*> actions = m_appStyleActionGroup->actions();
    for (auto action : actions) {
        m_appStyleActionGroup->removeAction(action);
    }

    QMenu* appStyleMenu = new QMenu(tr("Application Style"), this);
    m_optionMenu->addMenu(appStyleMenu);
    QStringList keys = QStyleFactory::keys();
    QString style = xToolsSettings::instance()->appStyle();
    if (style.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 1, 0)
        style = qApp->style()->name();
#endif
    }

    for (QString& key : keys) {
        QAction* action = new QAction(key, this);
        action->setObjectName(key);
        action->setCheckable(true);
        m_appStyleActionGroup->addAction(action);

        if (key == style) {
            action->setChecked(true);
        }

        connect(action, &QAction::triggered, this, [=]() {
            xToolsSettings::instance()->setAppStyle(key);
            tryToReboot();
        });
    }

    appStyleMenu->addActions(m_appStyleActionGroup->actions());
}

void xToolsMainWindow::initOptionMenuSettingsMenu()
{
    QMenu* menu = new QMenu(tr("Settings"), this);
    m_optionMenu->addMenu(menu);

    QAction* action = new QAction(tr("Clear Configuration"), this);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, [=]() {
        xToolsSettings::instance()->setClearSettings(true);
        tryToReboot();
    });
    action = new QAction(tr("Open configuration floder"), this);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, [=]() {
        QString file_name = xToolsSettings::instance()->fileName();
        QUrl file_url = QUrl(file_name);
        QString floder_url = file_name.remove(file_url.fileName());
        QDesktopServices::openUrl(floder_url);
    });
}

void xToolsMainWindow::initOptionMenuHdpiPolicy()
{
    QMenu* menu = new QMenu(tr("HDPI Policy"));
    QActionGroup* action_group = new QActionGroup(this);
    int setting_policy = xToolsSettings::instance()->hdpiPolicy();
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QList<QPair<int, QString>> policy_list;
    typedef QPair<int, QString> policy_pair;
    policy_list << policy_pair(xToolsDataStructure::HdpiPolicyRound, tr("Round up for .5 and above"));
    policy_list << policy_pair(xToolsDataStructure::HdpiPolicyCeil, tr("Always round up"));
    policy_list << policy_pair(xToolsDataStructure::HdpiPolicyFloor, tr("Always round down"));
    policy_list << policy_pair(xToolsDataStructure::HdpiPolicyRoundPreferFloor,
                               tr("Round up for .75 and above"));
    policy_list << policy_pair(xToolsDataStructure::HdpiPolicyPassThrough, tr("Don't round"));
    for (auto& policy : policy_list) {
        QAction* action = new QAction(policy.second, this);
        action_group->addAction(action);
        if (setting_policy == policy.first) {
            action->setCheckable(true);
        }

        connect(action, &QAction::triggered, this, [=]() {
            onHdpiPolicyActionTriggered(policy.first);
        });
    }
    menu->addActions(action_group->actions());
    m_optionMenu->addMenu(menu);
#endif

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QAction* system_action = new QAction(tr("System"), this);
    system_action->setCheckable(true);
    action_group->addAction(system_action);
    menu->addSeparator();
    menu->addAction(system_action);

    if (setting_policy == xToolsDataStructure::HdpiPolicySystem) {
        system_action->setChecked(true);
    }

    connect(system_action, &QAction::triggered, this, [=]() {
        xToolsSettings::instance()->setHdpiPolicy(xToolsDataStructure::HdpiPolicySystem);
        createQtConf();
        tryToReboot();
    });
#endif
#endif
}

void xToolsMainWindow::onHdpiPolicyActionTriggered(int policy)
{
    if (QFile::remove(getQtConfFileName())) {
        qInfo() << getQtConfFileName() << "was removed!";
    } else {
        qInfo() << "removed" << getQtConfFileName() << "failed";
    }

    xToolsSettings::instance()->setHdpiPolicy(int(policy));
    tryToReboot();
}

void xToolsMainWindow::onGithubActionTriggered()
{
    QDesktopServices::openUrl(QUrl(SAK_GITHUB_REPOSITORY_URL));
}

void xToolsMainWindow::onGiteeActionTriggered()
{
    QDesktopServices::openUrl(QUrl(SAK_GITEE_REPOSITORY_URL));
}

void xToolsMainWindow::onUserQqGroupTriggerd()
{
    QPixmap pix;
    if (!pix.load(":/resources/images/QSAKQQ.jpg")) {
        qWarning() << "Can not load QSAKQQ.jpg.";
        return;
    }

    QLabel* label = new QLabel(this);
    label->resize(pix.size());
    label->setPixmap(pix);
    QDialog dialog;
    dialog.setLayout(new QHBoxLayout());
    dialog.layout()->addWidget(label);
    dialog.setModal(true);
    dialog.exec();
}

void xToolsMainWindow::onAboutActionTriggered()
{
    QString year = xToolsInterface::buildDateTime("yyyy");
    QString info;
    info += centralWidget()->windowTitle() + tr("(Part of Qt Swiss Army knife)");
    info += "\n";
    info += tr("Author: ") + SAK_AUTHOR;
    info += "\n";
    info += tr("Email: ") + SAK_AUTHOR_EMAIL;
    info += "\n";
#ifdef SAK_GIT_COMMIT
    info += tr("Commit: ") + SAK_GIT_COMMIT;
#endif
    info += "\n";
#ifdef SAK_GIT_COMMIT_TIME
    info += tr("Date: ") + SAK_GIT_COMMIT_TIME;
#endif
    info += "\n";
    info += tr("Copyright 2018-%1 x-tools-author(x-tools@outlook.com). All rights reserved.").arg(year);
    QMessageBox::about(this, tr("About"), info);
}

void xToolsMainWindow::tryToReboot()
{
    int ret = QMessageBox::information(this,
                                       tr("Reboot application to effective"),
                                       tr("Need to reboot, reboot to effective now?"),
                                       QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {
        QProcess::startDetached(QCoreApplication::applicationFilePath());

        qApp->closeAllWindows();
        qApp->exit();
    }
}

void xToolsMainWindow::createQtConf()
{
    QString fileName = getQtConfFileName();
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
        QTextStream out(&file);
        out << "[Platforms]\nWindowsArguments = dpiawareness=0\n";
        file.close();
    } else {
        qWarning() << fileName;
        qWarning() << "can not open file:" << file.errorString();
    }
}

QString xToolsMainWindow::getQtConfFileName()
{
    return qApp->applicationDirPath() + "/qt.conf";
}