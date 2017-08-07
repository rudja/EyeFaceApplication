call haspdinst_nokey -install
timeout /t 5
for %%i in (.\trial-keys\*) do (
echo Installing %%i... 
call hasp-update u %%i
)
timeout /t 5
