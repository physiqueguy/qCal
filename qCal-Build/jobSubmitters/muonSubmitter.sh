### Job Runner for qCal
### Contact: james-wetzel@uiowa.edu
###
### $1 = particle (muon, pion, electron, for filename)
### $2 = energy (1, 5, 25, 50, etc., in GeV)
### $3 = location of qCal Directory on the cluster
### $4 = number of events per job
### $5 = number of job per energy (which is the seed)

echo "/run/initialize" 				>> $1_$2_GeV.mac
echo "/control/verbose 0" 			>> $1_$2_GeV.mac
echo "/run/verbose 0" 				>> $1_$2_GeV.mac
echo "/event/verbose 0" 			>> $1_$2_GeV.mac
echo "/tracking/verbose 0" 			>> $1_$2_GeV.mac
echo "/gun/particle mu-" 			>> $1_$2_GeV.mac
echo "/run/printProgress 500" 			>> $1_$2_GeV.mac
echo "/qCal/outputFileName $1_$2-GeV_$4" 	>> $1_$2_GeV.mac
echo "/run/beamOn $4" 				>> $1_$2_GeV.mac

echo "$3/qCal -m $3/jobSubmitters/$1_$2_GeV.mac -e $2 -s $5" >> $1_$2_GeV.job
echo "mv $1_$2-GeV_$4.root $3/outputData/muons" 	>> $1_$2_GeV.job

qsub -V -j y -l mf=2G -o $3/jobSubmitters/ $1_$2_GeV.job
