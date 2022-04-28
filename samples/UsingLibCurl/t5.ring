load "libcurl.ring"

curl = curl_easy_init()

curl_easy_setopt(curl, CURLOPT_USERNAME, "demo");
curl_easy_setopt(curl, CURLOPT_PASSWORD, "password");

curl_easy_setopt(curl, CURLOPT_URL, "sftp://test.rebex.net:22")

See "connecting..."
cOutput = curl_easy_perform_silent(curl)
if Len(cOutput) != 0
	See "done." + nl

	See "Output:" + nl
	see cOutput
else
	See "FAILED!" + nl
ok

curl_easy_cleanup(curl)
