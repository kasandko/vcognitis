# Define the build version

top_srcdir = $$absolute_path($$PWD)
version_path = $$relative_path($$top_srcdir/version.h, $$top_builddir)

win32 {
    versionTarget.commands = $$top_srcdir/../scripts/version.bat $$top_srcdir
}
unix {
    versionTarget.commands = $$top_srcdir/../scripts/version.sh $$top_srcdir
}

versionTarget.target = $$version_path
# just rebuild it every time
versionTarget.depends = FORCE

PRE_TARGETDEPS += $$version_path
QMAKE_EXTRA_TARGETS += versionTarget

HEADERS += $$version_path
INCLUDEPATH += $$top_srcdir/
