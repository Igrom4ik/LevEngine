# Check produced package (.zip or .tar.gz) for presence of source files (*.cpp, *.h) and for README.md
param(
    [string]$PackageDir = "${PWD}/build/package"
)

Write-Host "Looking for packages in: $PackageDir"
if (-Not (Test-Path $PackageDir)) {
    Write-Error "Package directory not found: $PackageDir"
    exit 2
}
$zip = Get-ChildItem -Path $PackageDir -Include *.zip -Recurse -File -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $zip) { $zip = Get-ChildItem -Path $PackageDir -Include *.tar.gz,*.tgz -Recurse -File -ErrorAction SilentlyContinue | Select-Object -First 1 }
if (-not $zip) {
    Write-Error "No package archive found in $PackageDir"
    exit 2
}
Write-Host "Found package: $($zip.FullName)"

Write-Host "Archive listing:"
if ($has7z) {
    & 7z l -ba $zip.FullName | ForEach-Object { Write-Host $_ }
} elseif ($zip.Extension -eq ".zip") {
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    $zipArchive = [System.IO.Compression.ZipFile]::OpenRead($zip.FullName)
    foreach ($entry in $zipArchive.Entries) { Write-Host $entry.FullName }
    $zipArchive.Dispose()
} else {
    & tar -tzf $zip.FullName 2>$null | ForEach-Object { Write-Host $_ }
}

# Use 7zip if available, otherwise use System.IO.Compression for zip
$has7z = (Get-Command 7z -ErrorAction SilentlyContinue) -ne $null
$containsSources = $false
$containsReadme = $false

if ($zip.Extension -eq ".zip") {
    if ($has7z) {
        $list = & 7z l -ba $zip.FullName | ForEach-Object { $_ }
        # find lines that look like file entries
        foreach ($l in $list) {
            if ($l -match "\.cpp$|\.c$|\.h$|\.hpp$") { $containsSources = $true; break }
            if ($l -match "(?i)README(\.md)?$") { $containsReadme = $true }
        }
    } else {
        Add-Type -AssemblyName System.IO.Compression.FileSystem
        $zipArchive = [System.IO.Compression.ZipFile]::OpenRead($zip.FullName)
        foreach ($entry in $zipArchive.Entries) {
            if ($entry.FullName -match "\.cpp$|\.c$|\.h$|\.hpp$") { $containsSources = $true; break }
            if ($entry.FullName -match "(?i)README(\.md)?$") { $containsReadme = $true }
        }
        $zipArchive.Dispose()
    }
} else {
    # tar.gz check - extract listing via tar
    $list = & tar -tzf $zip.FullName 2>$null
    foreach ($l in $list) {
        if ($l -match "\.cpp$|\.c$|\.h$|\.hpp$") { $containsSources = $true; break }
        if ($l -match "(?i)README(\.md)?$") { $containsReadme = $true }
    }
}

if ($containsSources) {
    Write-Error "Source files (*.cpp/*.h) found in package"
    exit 1
}
if (-not $containsReadme) {
    Write-Error "README.md not found in archive"
    exit 1
}
Write-Host "Package verification passed: no source files, README.md present"
exit 0
