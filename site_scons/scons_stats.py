import pstats
import sys

stats = pstats.Stats(sys.argv[1])
stats.sort_stats('time','name').print_stats()

