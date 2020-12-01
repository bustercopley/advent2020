## Usage:
##
## ./get-input.sh 01
##
## Save ${URL}/day/1/input to file input/01, using the session cookie in file ".cookie".
## To get the cookie, use a browser to sign in, then inspect the headers for a page request.

URL=https://adventofcode.com/2020
if [ -e .cookie ]; then
  read cookie < .cookie
  curl -s -o "input/$1" "${URL}/day/$((10#$1))/input" -X GET -H "Cookie: session=${cookie}"
else
  echo "Sign in to ${URL} to get a cookie!"
fi
