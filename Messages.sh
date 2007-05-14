#! /usr/bin/env bash
$EXTRACTRC *.rc *.ui *.kcfg > rc.cpp
: > levelnames.cpp
for i in levels/level_*; do
egrep '^Name=' $i | sed -e 's#^Name=\(.*\)$#i18n(\"\1\")#' >> levelnames.cpp
done
$XGETTEXT *.cpp -o $podir/katomic.pot
rm -f levelnames.cpp
