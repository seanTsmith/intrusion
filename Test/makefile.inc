INSTALLDIR=..\InstallImage

!if "$(_BUILDARCH)" == "x86" 
CPUDIR = $(INSTALLDIR)\i386
!else
CPUDIR = $(INSTALLDIR)\$(_BUILDARCH)
!endif

COPYTARGET:
        if not exist $(CPUDIR) (md $(CPUDIR) )
        if exist $(OBJ_PATH)\$(O)\$(TARGETNAME).exe copy $(OBJ_PATH)\$(O)\$(TARGETNAME).exe $(CPUDIR)
