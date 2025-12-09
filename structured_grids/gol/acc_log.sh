# produces static_report.log
rm -f acc_report.log

for i in {1..10}; do
  echo "=== RUN $i ===" >> acc_report.log
  ./gameoflife_acc -p >> acc_report.log
done
