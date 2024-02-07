<#
.SYNOPSIS
    This script Apply WinUSB to a USB device

.DESCRIPTION
    This script 1. Download WinUSB Installer Generator, 2. Generate WinUSB Installer, and 3. Apply WinUSB to a USB device with user provided vendor id
    
.PARAMETER VenderId
    The vender ID provided by the user.

.EXAMPLE
    .\generate_and_apply_WinUSB.ps1 <vendor id>
    Prompts the user for a vender ID and displays the entered vender ID.

.NOTES
    Author: Fixstars-momoko
    Date:   February 7, 2024
#>

param (
    [Parameter(Mandatory=$true)]
    [string]$VenderId
)

# Vender ID of the target USB device
Write-Host "You entered vender ID: $VenderId"

# Working Directory
$installPath = "$env:TEMP"
Write-Host "installPath = $installPath"


# Download
$installerName = "WinUSB-installer-generator"
$repoUrl = "https://api.github.com/repos/Sensing-Dev/$installerName/releases/latest"

$response = Invoke-RestMethod -Uri $repoUrl
$version = $response.tag_name

Write-Host "Latest version: $version" 

$Url = "https://github.com/Sensing-Dev/$installerName/releases/download/${version}/${installerName}-${version}.zip"
Write-Verbose "Download from: $Url"

# Extract Item
if ($Url.EndsWith("zip")) {
    # Download ZIP to a temp location

    $tempZipPath = "${env:TEMP}\${installerName}.zip"
    Invoke-WebRequest -Uri $Url -OutFile $tempZipPath -Verbose

    Add-Type -AssemblyName System.IO.Compression.FileSystem

    $tempExtractionPath = "$installPath\_tempWinUSBExtraction"
    # Create the temporary extraction directory if it doesn't exist
    if (Test-Path $tempExtractionPath) {
       Remove-Item -Path $tempExtractionPath -Force -Recurse -Confirm:$false
    }
    New-Item -Path $tempExtractionPath -ItemType Directory

    # Attempt to extract to the temporary extraction directory
    try {
        Expand-Archive -Path $tempZipPath -DestinationPath $tempExtractionPath 
        Get-ChildItem $tempExtractionPath
    }
    catch {
        Write-Error "Extraction failed...."
    }    
     Remove-Item -Path $tempZipPath -Force
}else{
    throw "Target archive does not exit."
}

# Generate WinUSB Installer
if (Test-Path $tempExtractionPath) {    

    # Run Winusb installer
    Write-Host "This may take a few minutes. Starting the installation..."

    Write-Verbose "Start winUsb installer"
    $TempDir = "$tempExtractionPath/temp"
        
    New-item -Path "$TempDir" -ItemType Directory
    $winUSBOptions = @{
        FilePath               = "${tempExtractionPath}/${installerName}-${versionNum}/winusb_installer.exe"
        ArgumentList           = "$VenderId"
        WorkingDirectory       = "$TempDir"
        Wait                   = $true
        Verb                   = "RunAs"  # This attempts to run the process as an administrator
    }
    # Start winusb_installer.exe process 
    Start-Process @winUSBOptions 

    Write-Verbose "End winUsb installer"
}else{
    throw "Failed to execute WinUSB Installer Generator."
}

# Run Driver installer
Write-Verbose "Start Driver installer"

$infPath = "$TempDir/target_device.inf"
if (-not (Test-Path -Path $infPath -PathType Leaf) ){
    Write-Error "$infPath does not exist."
}
else{
    $pnputilOptions = @{
        FilePath = "PNPUtil"
        ArgumentList           = "-i -a ./target_device.inf"
        WorkingDirectory       = "$TempDir"
        Wait                   = $true
        Verb                   = "RunAs"  # This attempts to run the process as an administrator
    }
    try {
        # Start Pnputil process 
        Start-Process @pnputilOptions 
        Write-Host "Sucessfully installed winUSB driver"
    }
    catch {
        Write-Error "An error occurred while running pnputil: $_"
    }
}

# delete temp files

Start-Process powershell -Verb RunAs -ArgumentList "-NoProfile -ExecutionPolicy Bypass -Command `"Remove-Item -Path '$tempExtractionPath' -Recurse -Force -Confirm:`$false`""

Write-Verbose "End Driver installer"