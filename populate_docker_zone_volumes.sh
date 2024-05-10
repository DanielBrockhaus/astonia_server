#!/bin/bash
# USAGE: Run this script in the directory where your zone information is stored.
# Example: You have a directory named "zones" with folders named 1 through 37.
# cd to that directory and execute this script in that directory.
# This is required for docker compose to bring up the game servers.
declare -A AREAS
AREAS["1"]="cameron"
AREAS["2"]="under_aston"
AREAS["3"]="aston"
AREAS["4"]="earth_pents"
AREAS["5"]="sewers"
AREAS["6"]="earth_underground"
AREAS["7"]="fire_pents"
AREAS["8"]="fire_underground"
AREAS["9"]="ice_pents"
AREAS["10"]="ice_underground"
AREAS["11"]="ice_palace"
AREAS["12"]="mines"
AREAS["13"]="catacombs"
AREAS["14"]="random_dungeons"
AREAS["15"]="swamps"
AREAS["16"]="exkordon_forest"
AREAS["17"]="exkordon"
AREAS["18"]="bone_tower"
AREAS["19"]="nomad_plains"
AREAS["20"]="live_quest"
AREAS["21"]="test_pents"
AREAS["22"]="labyrinth"
AREAS["23"]="ice_army_cave"
AREAS["24"]="more_ice_army_caves"
AREAS["25"]="rodneys_warped_world"
AREAS["26"]="below_aston_2"
AREAS["28"]="brannington_forest"
AREAS["29"]="brannington"
AREAS["30"]="clan_spawners"
AREAS["31"]="grimroot"
AREAS["32"]="jobbington"
AREAS["33"]="long_tunnel"
AREAS["34"]="teufelheim"
AREAS["36"]="caligar"
AREAS["37"]="arkhata"
for i in "${!AREAS[@]}"
do
  docker volume create ${AREAS[$i]}_zone_data
  tar -C $(pwd)/${i} -c -f- . | docker run --rm -i -v ${AREAS[$i]}_zone_data:/data alpine tar -C /data -xv -f-
done
docker volume create generic_zone_data
tar -C $(pwd)/generic -c -f- . | docker run --rm -i -v generic_zone_data:/data alpine tar -C /data -xv -f-
docker volume create preset_zone_data
tar -C $(pwd)/presets -c -f- . | docker run --rm -i -v preset_zone_data:/data alpine tar -C /data -xv -f-
