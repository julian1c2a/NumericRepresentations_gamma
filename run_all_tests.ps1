#!/usr/bin/env pwsh
# Script para ejecutar todos los tests y mostrar un resumen

Write-Host "=================================="
Write-Host "EJECUTANDO TODOS LOS TESTS"  
Write-Host "=================================="

$testDir = "./builddir/tests"
$testFiles = Get-ChildItem -Path "$testDir/*.exe" | Sort-Object Name
$totalTests = 0
$totalAssertions = 0
$passedTests = 0

foreach ($testFile in $testFiles) {
    $testName = $testFile.Name
    Write-Host "`n--- Ejecutando: $testName ---"
    
    try {
        $output = & $testFile.FullName 2>&1
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✅ PASÓ" -ForegroundColor Green
            $passedTests++
            
            # Extraer número de assertions si aparece en la salida
            if ($output -match "All tests passed \((\d+) assertions? in \d+ test cases?\)") {
                $assertions = [int]$matches[1]
                $totalAssertions += $assertions
                Write-Host "   Assertions: $assertions" -ForegroundColor Cyan
            }
        } else {
            Write-Host "❌ FALLÓ" -ForegroundColor Red
            Write-Host $output
        }
    } catch {
        Write-Host "❌ ERROR AL EJECUTAR" -ForegroundColor Red
        Write-Host $_.Exception.Message
    }
    
    $totalTests++
}

Write-Host "`n=================================="
Write-Host "RESUMEN FINAL" -ForegroundColor Yellow
Write-Host "=================================="
Write-Host "Tests ejecutados: $totalTests" -ForegroundColor White
Write-Host "Tests exitosos: $passedTests" -ForegroundColor Green  
Write-Host "Tests fallidos: $($totalTests - $passedTests)" -ForegroundColor Red
Write-Host "Total assertions: $totalAssertions" -ForegroundColor Cyan

if ($passedTests -eq $totalTests) {
    Write-Host "`n🎉 ¡TODOS LOS TESTS PASARON! 🎉" -ForegroundColor Green
} else {
    Write-Host "`n⚠️  Algunos tests fallaron" -ForegroundColor Yellow
}