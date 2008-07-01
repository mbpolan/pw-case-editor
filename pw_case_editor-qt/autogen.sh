#!/bin/bash
cd intl
lrelease pw_case_editor-qt_pl.ts
cd ../
qmake
qmake -spec macx-g++
