
# This file must be blank output except for the pid. Its used to kill later...

$out_dir = $args[0] #-replace("/","\")
$exe = $args[1] #-replace("/","\")

$numOfArgs = $args.Length - 1
$exe_args = ""
for ($i=2; $i -le $numOfArgs; $i++)
{
    $exe_args = $exe_args + " " +  $args[$i]
}
#Write-Host $args
#Write-Host "Starting start_background.ps1 with |$exe| and |$out_dir| and args |$my_args|"

# TODO - find a way to get stdout/stderr... if use redirect it blocks the process and waits
#        Eg still use this script with start_interrupt but pass "none" in logdir and use wait condition file instead
#        Or switch to python...

#$app = Start-Process -passthru -FilePath "$exe" -ArgumentList "$exe_args" -RedirectStandardOutput "$out_dir/stdout.log" -RedirectStandardError "$out_dir/stderr.log"
$app = Start-Process -passthru -FilePath "$exe" -ArgumentList "$exe_args"
Write-Host $app.Id
