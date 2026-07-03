$ErrorActionPreference = "Stop"

$root = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$webRoot = Join-Path $root "lv_web_emscripten\cmbuild"
$node = Join-Path $root "emsdk\node\22.16.0_64bit\bin\node.exe"
$python = Join-Path $root "emsdk\python\3.13.3_64bit\python.exe"
$proxy = Join-Path $root "lv_web_emscripten\tools\stream_proxy.mjs"

if (!(Test-Path $webRoot)) {
  throw "Build output not found: $webRoot"
}

Write-Host "[sim] starting stream proxy on http://127.0.0.1:8092"
Start-Process -FilePath $node -ArgumentList @($proxy) -WorkingDirectory $root -WindowStyle Hidden

Write-Host "[sim] starting web server on http://127.0.0.1:8091"
Start-Process -FilePath $python -ArgumentList @("-m", "http.server", "8091", "--bind", "127.0.0.1") -WorkingDirectory $webRoot -WindowStyle Hidden

Write-Host "[sim] open http://127.0.0.1:8091/esp_claw_sim.html"
