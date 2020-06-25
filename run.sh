# Build
make


./rtree Testcases/TC_6/queries_2_10_100_100.txt 10 2 output_tc6.txt
cmp --silent output_tc6.txt Testcases/TC_1/answer_2_10_100_100_bulk.txt && echo 'TC6: SUCCESS' || echo 'TC6: FAILED'
rm -f output_tc6.txt
rm -f RTREE_INDEX.txt

# ./rtree query.txt 10 2 output.txt
# rm -f RTREE_INDEX.txt
# cmp --silent output.txt answer.txt && echo 'TC6: SUCCESS' || echo 'TC6: FAILED'
