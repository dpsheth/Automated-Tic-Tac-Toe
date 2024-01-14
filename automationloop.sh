#!/bin/bash

MQTT_SERVER="nickhoang.duckdns.org"
MQTT_PORT="1883"
PUB_TOPIC="random_numbers"
SUB_TOPIC="game_results"

file_path="/home/nhoang4227/file.txt"
numbers=(1 2 3 4 5 6 7 8 9)

echo "Starting script"
# Create the file if it doesn't exist and initialize with 0s
if [ ! -f "$file_path" ]; then
    echo "0" > "$file_path"
    echo "0" >> "$file_path"
    echo "0" >> "$file_path"
fi

# MQTT subscriber function
mqtt_subscribe() {
        echo "Waiting for message..."
        message=$(mosquitto_sub -h $MQTT_SERVER -t $SUB_TOPIC -C 1)
        echo "Received message: $message"

        if [ "$message" == "W" ]; then
            awk '{ if (NR==1) $1+=1; print $0 }' "$file_path" > temp.txt && mv temp.txt "$file_path"
        elif [ "$message" == "L" ]; then
            awk '{ if (NR==2) $1+=1; print $0 }' "$file_path" > temp.txt && mv temp.txt "$file_path"
        elif [ "$message" == "D" ]; then
            awk '{ if (NR==3) $1+=1; print $0 }' "$file_path" > temp.txt && mv temp.txt "$file_path"
        fi
}

shuffle_array() {
    local array=("$@")
    local len=${#array[@]}
    for ((i=len-1; i>0; i--)); do
        local j=$((RANDOM % (i+1)))
        local temp=${array[i]}
        array[i]=${array[j]}
        array[j]=$temp
    done
    echo "${array[@]}"
}
for ((i = 0; i < 100; i++)); do
     shuffled=($(shuffle_array "${numbers[@]}"))
     for num in "${shuffled[@]}"; do
     # Publish number to MQTT
        echo "Publishing number"
         mosquitto_pub -h $MQTT_SERVER -p $MQTT_PORT -t $PUB_TOPIC -m "$num"

# Add a delay between publishes (adjust as needed)
         sleep .125
     done
     mqtt_subscribe
done
