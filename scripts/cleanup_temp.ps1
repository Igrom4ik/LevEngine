# scripts/cleanup_temp.ps1
# Safely remove temporary build/test directories created during development
$patterns = @('build_validate*', 'build_version_test*', 'build_temp', 'pkg_extract*', 'staging*', 'package*')
$root = (Resolve-Path ".").Path
Write-Host "Cleanup root: $root"
foreach ($pat in $patterns)
{
    $found = Get-ChildItem -Path $root -Filter $pat -Force -ErrorAction SilentlyContinue
    if ($found)
    {
        foreach ($item in $found)
        {
            try
            {
                Remove-Item -LiteralPath $item.FullName -Recurse -Force -ErrorAction Stop
                Write-Host "Removed: $( $item.FullName )"
            }
            catch
            {
                Write-Warning "Failed to remove $( $item.FullName ): $( $_.Exception.Message )"
            }
        }
    }
    else
    {
        Write-Host "Not found pattern: $pat"
    }
}
Write-Host "Cleanup done."
