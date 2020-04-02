
set baseDir=%1
set output=%baseDir%/version.h

FOR /F "tokens=* USEBACKQ" %%g IN (`git --git-dir %baseDir%/../.git --work-tree %baseDir% describe --abbrev^=0 --tags`) do (SET "baseVersion=%%g")
FOR /F "tokens=* USEBACKQ" %%g IN (`git --git-dir %baseDir%/../.git --work-tree %baseDir% rev-parse --short HEAD`) do (SET "baseCommit=%%g")

@echo #pragma once > %output%
@echo #define GIT_VERSION ^"%baseVersion%^" >> %output%
@echo #define GIT_COMMIT ^"%baseCommit%^" >> %output%
