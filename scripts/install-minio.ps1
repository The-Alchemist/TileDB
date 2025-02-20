<#
.SYNOPSIS
This is a Powershell script to install the Minio server on Windows.

.DESCRIPTION
This is a Powershell script to install the Minio server on Windows.

.LINK
https://github.com/TileDB-Inc/TileDB
#>

[CmdletBinding()]
Param(
)

# Return the directory containing this script file.
function Get-ScriptsDirectory {
    Split-Path -Parent $PSCommandPath
}

$TileDBRootDirectory = Split-Path -Parent (Get-ScriptsDirectory)
$InstallPrefix = Join-Path $TileDBRootDirectory "dist"
$StagingDirectory = Join-Path (Get-ScriptsDirectory) "deps-staging"

New-Item -ItemType Directory -Path (Join-Path $InstallPrefix bin) -ea 0

function DownloadFile([string] $URL, [string] $Dest) {
    Write-Host "Downloading $URL to $Dest..."
    Invoke-WebRequest -Uri $URL -OutFile $Dest
}

function DownloadIfNotExists([string] $Path, [string] $URL) {
    if (!(Test-Path $Path)) {
	# Minio uses TLS1.2
	[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
	DownloadFile $URL $Path
    }
}
function Install-Minio {
    $MinioRoot = (Join-Path $StagingDirectory "minio")
    $DownloadMinioDest = Join-Path $MinioRoot "minio.exe"
    if (!(Test-Path $MinioRoot)) {
	New-Item -ItemType Directory -Path $MinioRoot
	DownloadIfNotExists $DownloadMinioDest "https://dl.minio.io/server/minio/release/windows-amd64/minio.exe"
    }
    Copy-Item $DownloadMinioDest (Join-Path $InstallPrefix "bin")
}

function Export-Env {
    $env:MINIO_ACCESS_KEY = "minio"
    $env:MINIO_SECRET_KEY = "miniosecretkey"
    $env:AWS_ACCESS_KEY_ID = "minio"
    $env:AWS_SECRET_ACCESS_KEY = "miniosecretkey"
}

function Run-Minio {
    $ExePath = Join-Path $InstallPrefix "bin\minio.exe"
    $ServerConfigDir = Join-Path $InstallPrefix "etc\minio"
    $ServerDataDir = Join-Path $InstallPrefix "usr\local\share\minio"
    if (!(Test-Path $ServerDataDir)) {
	New-Item -ItemType Directory -Path $ServerDataDir
    }
    if (!(Test-Path $ServerConfigDir)) {
	New-Item -ItemType Directory -Path $ServerConfigDir
    }
    Start-Process -FilePath $ExePath -ArgumentList "server --address 127.0.0.1:9999 --config-dir `"$ServerConfigDir`" `"$ServerDataDir`""
}

function Install-All-Deps {
    if (!(Test-Path $StagingDirectory)) {
	New-Item -ItemType Directory -Path $StagingDirectory
    }
    Install-Minio
    Export-Env
    Run-Minio
}

Install-All-Deps

Write-Host "Finished."
