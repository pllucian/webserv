from os import getenv
from urllib.parse import parse_qs


name = "anonymous"
age = "no age"
data = parse_qs(getenv("QUERY_STRING"))
if "name" in data:
    name = data["name"][0]
if "age" in data:
    age = data["age"][0]
content_type = getenv("CONTENT_TYPE")
print("""<!
<html>
<head>
<title>Welcome to website0!</title>
<meta charset="utf-8">
<style>
    body {
        width: 35em;
        margin: 0 auto;
        font-family: Tahoma, Verdana, Arial, sans-serif;
    }
</style>
</head>
<body>
<h1>Welcome to website0!</h1>""")
if content_type == "multipart/form-data":
    print("<p>File upload is successful!</p>")
else:
    print("<p>Name: " + name + "</p>")
    print("<p>Age: " + age + "</p>")
print("""<p><a href="/">back</a></p>
</body>
</html>""")
