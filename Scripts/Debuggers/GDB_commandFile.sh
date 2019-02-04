install=home/bebenlebricolo/repos/FreeMyCode/Install
fmc_bin=$[install]/bin/FreeMyCode
target=/home/bebenlebricolo/repos/FreeMyCode/Sources
res_dir=/home/bebenlebricolo/repos/FreeMyCode/UserRessourcesDir
config=${install}/Ressources/Config.json


${fmc_bin} ${target} -A ${res_dir}/License.txt \
-p -c ${config} -L ${res_dir}/logfile.log \
-Si ${res_dir}/Secondary_input.json \
-Sd /${install}/Ressources/Spectrums

break Spectrum::compareWithSpectrum