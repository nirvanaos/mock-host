if ($args.count -ge 1) {
	$platform = $args[0]
} else {
	$platform = "x64"
}
if ($args.count -ge 2) {
	$config = $args[1]
} else {
	$config = "Debug"
}

$ErrorActionPreference = "Stop"

& "$PSScriptRoot\vsdevshell.ps1"

switch ($platform) {
	"x64" {
		$arch = "x86_64"
		Enter-VsDevShell -VsInstallPath:"$visualStudioPath" -SkipAutomaticLocation -HostArch amd64 -Arch amd64
	}
	"x86" {
		$arch = "i686"
		Enter-VsDevShell -VsInstallPath:"$visualStudioPath" -SkipAutomaticLocation -HostArch amd64 -Arch x86
	}
	default {
		Write-Host "Unknown platform"
		Exit -1
	}
}

$build_dir = "$PWD\build\$platform-$config"

cmake -G Ninja -S . -B $build_dir `
 -DCMAKE_BUILD_TYPE="$config"

cmake --build $build_dir
