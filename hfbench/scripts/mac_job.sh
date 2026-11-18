#!/bin/bash
# Job runner (macOS, native). Polls hfbench/queue for *.sh jobs and runs them
# one at a time. Each job's output goes to queue/done/<job>.log; exit code to
# queue/done/<job>.rc. Stops when hfbench/queue/STOP exists or after 4 h idle.
cd "$(dirname "$0")/.."           # -> hfbench
Q=queue; D=queue/done
idle=0
echo "runner started $(date -u +%FT%TZ) pid=$$" > $Q/RUNNER_ALIVE
while true; do
  [ -f $Q/STOP ] && { rm -f $Q/STOP $Q/RUNNER_ALIVE; echo "stopped"; exit 0; }
  job=$(ls $Q/*.sh 2>/dev/null | sort | head -1)
  if [ -n "$job" ]; then
    name=$(basename "$job" .sh)
    mv "$job" "$D/$name.sh"
    echo "[$(date +%T)] start $name"
    echo "$name $(date -u +%FT%TZ)" > $Q/RUNNING
    ( cd .. && bash "hfbench/$D/$name.sh" ) > "$D/$name.log" 2>&1
    echo $? > "$D/$name.rc"
    rm -f $Q/RUNNING
    echo "[$(date +%T)] end $name rc=$(cat $D/$name.rc)"
    idle=0
  else
    sleep 3; idle=$((idle+3))
    date -u +%FT%TZ > $Q/HEARTBEAT
    [ $idle -gt 14400 ] && { rm -f $Q/RUNNER_ALIVE; echo "idle timeout"; exit 0; }
  fi
done
