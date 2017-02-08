# fmap
# (c) 2015 Andrzej Budzanowski

# tabela importów
import argparse
import tempfile
import os
import subprocess
import shutil

print("fmap")
print("(c) 2015 Andrzej Budzanowski")

# argumenty
parser = argparse.ArgumentParser(description='processing multiple archives into collection')
parser.add_argument("-r", "--rar-path", metavar='PATH', help='Path to unrar.exe', default='unrar.exe')
parser.add_argument("-t", "--temp-path", metavar='PATH', help='Path to temporary directory', default=tempfile.gettempdir())
parser.add_argument("-m", "--max-chars", metavar='CHARS', help='Amount of chars', default=3, type=int)
parser.add_argument("--dont-delete", dest='delete_files', action='store_false', help="Don't delete files after processing")
parser.add_argument("--do-delete", dest='delete_files', action='store_true', help="Delete files after processing (DEFAULT)")
parser.add_argument("directory", help='Directory to move processed archives. If directory don\'t exist it will be created')
parser.add_argument("rars", help='Archives to process', nargs='+', metavar='N')
args = parser.parse_args()

if not os.path.isdir(args.temp_path):
 print("Temporary path doesn't exist!")
 print(" Path: ", args.temp_path)
 exit(2)
 
FNULL = open( os.devnull, 'w' )

try:
 subprocess.call(args.rar_path, stdout=FNULL, stderr=FNULL );
except:
 print("Unrar not found!")
 exit(3)

START_FROM_NUMBER = 0
CURRENT           = 0
# sprawdzamy czy directory jest utworzone
if not os.path.isdir(args.directory):
 try:
  os.mkdir( args.directory )
 except:
  print("Can't create directory!")
  print(" Directory name: ", args.directory)
  exit(4)
else:
 # sprawdzamy największy numer w podkatalogach
 for x in os.listdir( args.directory ):
  if os.path.isdir( os.path.join(args.directory,x) ):
   try:
    CURRENT = int(x, base=10)
   except ValueError:
    continue
   START_FROM_NUMBER = max( START_FROM_NUMBER, CURRENT )
   
START_FROM_NUMBER += 1

def get_basename( file ):
 return os.path.splitext( os.path.basename( file ))[0]

def extract_file( file, tmp_folder, rar_path, dir, delfiles, dirnumber, format_dir ):
 # tworzymy katalog
 str_path = os.path.join( dir, str(dirnumber).rjust(format_dir, '0') )
 os.mkdir( str_path )
 
 # wyładowanie pliku
 print("Extracting file: ", file);
 subprocess.call( [ rar_path, 'x', '-ac', '-ad', '-o+', file, tmp_folder ], stdout=FNULL, stderr=FNULL )
 
 print("Populate directory:", str_path)
 for x in os.walk( os.path.join(tmp_folder, get_basename(file) ) ):
  for y in x[2]:
   shutil.move( os.path.join(x[0], y), os.path.join(str_path, y))
 
 print("Remove temporary")
 shutil.rmtree( os.path.join( tmp_folder, get_basename(file) ) )
 
 if delfiles:
  os.remove( file )
 
for x in args.rars:
 extract_file( x, args.temp_path, args.rar_path, args.directory, args.delete_files, START_FROM_NUMBER, args.max_chars )
 START_FROM_NUMBER += 1
 
print("Done")
