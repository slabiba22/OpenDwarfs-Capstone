# produces static_report.log
rm -f mp_report.log

for i in {1..10}; do
  echo "=== RUN $i ===" >> mp_report.log
  ./gameoflife -p >> mp_report.log
done
