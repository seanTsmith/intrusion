_LNG=$(LANGUAGE)
_INX=.
!INCLUDE ..\include\packageversion.h
STAMP=stampinf -f $@ -a $(_BUILDARCH) -k $(KMDF_VERSION_MAJOR).$(KMDF_VERSION_MINOR) -d * -v $(STAMPINF_VERSION_STR)



$(OBJ_PATH)\$(O)\$(INF_NAME).inf: $(_INX)\$(INF_NAME).inx 
    copy $(_INX)\$(@B).inx $@
    $(STAMP)

INSTALLDIR=..\InstallImage

!if "$(_BUILDARCH)" == "x86" 
CPUDIR = $(INSTALLDIR)\i386
!else
CPUDIR = $(INSTALLDIR)\$(_BUILDARCH)
!endif

COPYTARGET:
        if not exist $(INSTALLDIR) (md $(INSTALLDIR) )
        if not exist $(CPUDIR) (md $(CPUDIR) )
        if exist $(OBJ_PATH)\$(O)\$(TARGETNAME).sys copy $(OBJ_PATH)\$(O)\$(TARGETNAME).sys $(CPUDIR)
        if exist $(OBJ_PATH)\$(O)\$(INF_NAME).inf copy $(OBJ_PATH)\$(O)\$(INF_NAME).inf $(CPUDIR)
