#!groovy​
properties([[$class: 'RebuildSettings', autoRebuild: false, rebuildDisabled: false], [$class: 'EnvInjectJobProperty', info: [loadFilesFromMaster: false, propertiesContent: '$(QTDIR)=C:\\tool\\RemotePkg\\Qt\\Qt5.6.0\\5.6\\msvc2013_64'], keepBuildVariables: true, keepJenkinsSystemVariables: true, on: true], pipelineTriggers([[$class: 'PeriodicFolderTrigger', interval: '1m']])])

	
node{	
	stage('scm')
	{
		git url: 'C:/Projekte/GitJenkinsTest/GitTest'
		bat 'git rev-parse --abbrev-ref HEAD > GIT_BRANCH'
		git_branch = readFile('GIT_BRANCH').trim()
		echo git_branch
		checkout scm
	}
	stage('build')
	{

		echo '##########################'
		echo 'Build step started'
		echo '##########################'
		bat "\"${tool 'MSBuild'}\" RemoteService.sln /p:Configuration=Debug /p:Platform=\"x64\""
		echo '##########################'
		echo 'Build step finished'
		echo '##########################'
	}

	stage('test')
	{
	echo 'Test step started'
	dir('QTTestApplication/x64/Debug/')
	{
	    //bat "UnitTestRemoteBox.exe"
		//step([$class: 'XUnitBuilder', testTimeMargin: '3000', thresholdMode: 1, thresholds: [[$class: 'FailedThreshold', failureNewThreshold: '', failureThreshold: '', unstableNewThreshold: '', unstableThreshold: ''], [$class: 'SkippedThreshold', failureNewThreshold: '', failureThreshold: '', unstableNewThreshold: '', unstableThreshold: '']], tools: [[$class: 'QTestLibType', deleteOutputFiles: true, failIfNotNew: true, pattern: '*.xml', skipNoTestFiles: false, stopProcessingIfError: true]]])
	}

	echo ' Test step finished'
	}

	stage('deploy')
	{
		echo '##########################'	
		echo 'Deploy step started'
		echo '##########################'
		bat "\"${tool 'Doxygen'}\" Doxyfile"

		
		echo '##########################'
		echo 'Deploy step finished'
		echo '##########################'
	}
}