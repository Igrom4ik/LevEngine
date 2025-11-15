# scripts/create_ci_tag.ps1
# Создаёт аннотированный CI-тег на основе версии в CMakeLists.txt и short SHA текущего коммита, затем пушит его в origin.
$cm = Get-Content -Path "CMakeLists.txt" -Raw -ErrorAction Stop
$ver = '0.0.0'
# Try to match set(PROJECT_VERSION "1.2.3") first
if ($cm -match 'set\s*\(\s*PROJECT_VERSION\s+"?([0-9]+\.[0-9]+\.[0-9]+)"?') {
    $ver = $matches[1]
} elseif ($cm -match 'project\s*\(\s*[^\s]+\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)') {
    $ver = $matches[1]
}
$sha = (git rev-parse --short HEAD) -replace '\r|\n', ''
$tag = "v${ver}-ci-${sha}"
# ensure tag is unique; if exists, append timestamp
if (git tag -l $tag) {
    $tag = "v${ver}-ci-${sha}-$( Get-Date -Format yyyyMMddHHmmss )"
}
Write-Host "Creating tag: $tag"
& git tag -a $tag -m "CI trigger: $tag"
$pushRes = & git push origin $tag 2>&1
if ($LASTEXITCODE -ne 0)
{
    Write-Error "Failed to push tag: $pushRes"
    exit $LASTEXITCODE
}
Write-Host "Created and pushed tag: $tag"
exit 0
