/*
 * This file is part of OpenWinControls.
 * Copyright (C) 2026 kylon
 *
 * OpenWinControls is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
#include <QApplication>
#include <QKeyEvent>
#include <QPushButton>
#include <QTest>

#include "../src/pages/backButtons/BackButtonsV1Page.h"
#include "../src/pages/faceButtons/KeyboardMouseButtonsPage.h"
#include "../src/pages/faceButtons/XinputButtonsPage.h"

namespace {
    QPushButton *firstMappingButton(QWidget &page) {
        const QList<QPushButton *> buttons = page.findChildren<QPushButton *>();

        for (QPushButton *button: buttons) {
            if (button->text().isEmpty())
                return button;
        }

        return nullptr;
    }

    void showPage(QWidget &page) {
        page.resize(1280, 800);
        page.show();
        QApplication::processEvents();
    }

    void sendKey(QWidget &page, const Qt::Key key) {
        QKeyEvent event(QEvent::KeyPress, key, Qt::NoModifier);
        QApplication::sendEvent(&page, &event);
    }
}

class KeyAssignmentE2ETest final: public QObject {
    Q_OBJECT

private slots:
    void keyboardMousePageRecordsDirectionKey() {
        OWC::KeyboardMouseButtonsPage page;
        showPage(page);

        QPushButton *button = firstMappingButton(page);
        QVERIFY(button != nullptr);
        QTest::mouseClick(button, Qt::LeftButton);
        QCOMPARE(button->text(), QStringLiteral("..."));
        QCOMPARE(QWidget::keyboardGrabber(), static_cast<QWidget *>(&page));

        sendKey(page, Qt::Key_Up);

        QCOMPARE(button->text(), QStringLiteral("UP"));
        QVERIFY(QWidget::keyboardGrabber() != &page);
    }

    void xinputPageConsumesKeyboardWithoutAssigningIt() {
        OWC::XinputButtonsPage page;
        showPage(page);

        QPushButton *button = firstMappingButton(page);
        QVERIFY(button != nullptr);
        QTest::mouseClick(button, Qt::LeftButton);
        button->setFocus();
        QWidget *focusBeforeKey = QApplication::focusWidget();
        QCOMPARE(button->text(), QStringLiteral("..."));
        QCOMPARE(QWidget::keyboardGrabber(), static_cast<QWidget *>(&page));

        sendKey(page, Qt::Key_Right);
        sendKey(page, Qt::Key_A);

        QCOMPARE(button->text(), QStringLiteral("..."));
        QCOMPARE(QApplication::focusWidget(), focusBeforeKey);

        page.setPendingButton(QStringLiteral("X_A"));
        QCOMPARE(button->text(), QStringLiteral("X_A"));
        QVERIFY(QWidget::keyboardGrabber() != &page);
    }

    void pendingCaptureResumesAfterKeyboardPageIsShownAgain() {
        OWC::KeyboardMouseButtonsPage page;
        showPage(page);

        QPushButton *button = firstMappingButton(page);
        QVERIFY(button != nullptr);
        QTest::mouseClick(button, Qt::LeftButton);
        QCOMPARE(QWidget::keyboardGrabber(), static_cast<QWidget *>(&page));

        page.hide();
        QApplication::processEvents();
        QVERIFY(QWidget::keyboardGrabber() != &page);

        showPage(page);
        QCOMPARE(QWidget::keyboardGrabber(), static_cast<QWidget *>(&page));

        sendKey(page, Qt::Key_Down);
        QCOMPARE(button->text(), QStringLiteral("DOWN"));
    }

    void pendingCaptureResumesAfterBackButtonsPageIsShownAgain() {
        OWC::BackButtonsV1Page page;
        showPage(page);

        QPushButton *button = firstMappingButton(page);
        QVERIFY(button != nullptr);
        QTest::mouseClick(button, Qt::LeftButton);
        QCOMPARE(QWidget::keyboardGrabber(), static_cast<QWidget *>(&page));

        page.hide();
        QApplication::processEvents();
        QVERIFY(QWidget::keyboardGrabber() != &page);

        showPage(page);
        QCOMPARE(QWidget::keyboardGrabber(), static_cast<QWidget *>(&page));

        sendKey(page, Qt::Key_Left);
        QCOMPARE(button->text(), QStringLiteral("LEFT"));
    }
};

QTEST_MAIN(KeyAssignmentE2ETest)

#include "KeyAssignmentE2ETest.moc"
