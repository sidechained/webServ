
Run server in one terminal, then execute following commands in separate terminal

- experiment with changing config i.e.
-- different routes
-- 
-- limit METHODS for certain paths

Testing with configs/servers.txt

# Standard Requests

## GET

### Should work:
`curl -X GET http://127.0.0.1:8080/page1.html`
`curl -X GET http://127.0.0.1:8080/Form.html`
`curl -X GET http://127.0.0.3:4545/index1.html`

## Shouldn't work:
`curl -X GET http://127.0.0.3:4500/index1.html`

### POST (content-length tests)

NOTE: default clientMaxBodySize is 1MB, but in our "configs/serversClientMaxBodySize.txt" i set it to 10MB

1. Under: Should not give "Content length exceeds client max body size." in output
`curl -X POST -H "Content-Type: application/octet-stream" -H "Content-Length: 500" --data-binary @input_file http://127.0.0.1:8080`

2. On border: Should not give "Content length exceeds client max body size." in output
`curl -X POST -H "Content-Type: application/octet-stream" -H "Content-Length: 1000" --data-binary @input_file http://127.0.0.1:8080`

3. Over: *Should* give "Content length exceeds client max body size." in output
`curl -X POST -H "Content-Type: application/octet-stream" -H "Content-Length: 50000" --data-binary @input_file http://127.0.0.1:8080`

## DELETE

not yet implemented
`curl -X DELETE http://127.0.0.1:8080/page1.html`

# CGI-php Requests

## GET (dynamic page generation)

`curl -X GET http://127.0.0.3:4500/dynamic.php`

## POST (uploads (multipart form request)

`curl -X POST -H "Content-Type: application/octet-stream" -H "Content-Length: 500" --data-binary @input_file http://127.0.0.1:8080`
`curl -X POST -H "Content-Type: application/octet-stream" -H "Content-Length: 500" --data-binary @input_file http://127.0.0.3:4545`

