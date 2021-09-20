if ("bin" -in (Get-ChildItem . -Name)) {
  Remove-Item "bin" -Recurse
}
mkdir "bin"

$calculi = @("untyped")

foreach ($calculus in $calculi) {
  $sources = Get-ChildItem ".\$calculus"
  $cfiles = @()

  foreach ($source in $sources) {
    if (($source.Extension -eq ".cpp") -or
        ($source.Extension -eq ".c")) {
      $cfiles += "..\$calculus\$source"
    }
  }

  Push-Location "bin"
  # cl /O2 /Fe$calculus.exe /I "../$calculus" $cfiles
  cl /Zi /Fe$calculus.exe /I "../$calculus" /I "../lib" $cfiles
  Pop-Location
}

