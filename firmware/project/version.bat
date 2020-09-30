FOR /F "tokens=3 delims= " %%x in (../drivers/bver.h) DO (
    set /a buildno=%%x+1
	echo %buildno%
)

set output=#define PLUGIN_BUILD %buildno%
echo %output% > ../drivers/bver.h
