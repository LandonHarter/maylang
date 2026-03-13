> **New:** Objects and arrays and stuff

# May

May is a perfect programming language. I created this language so I could write code exactly the way I like it.
Now, I won't have to follow the stupid standards of other programming languages and instead I will use my objectively
superior way of programming.

## Variables and Functions

There are 6 different variable types:

```js
var one;
int two;
float three;
string four;
object five;
array six;
```

While 5 of the 6 are typed, var can be used as an "any" type. Aditionally,
the var type can be used to define functions with no return value. For example:

```js
var do_something(string name, object parameters) {}
```

## Piping

Piping allows you to chain multiple functions together similar to bash. For example:

```js
import "io";
import "math";

sqrt(52) | floor | toString | print;
```

## Objects and Arrays

Objects and arrays work very similarly to how objects work in JavaScript. You can define an object
with braces and arrays with brackets. In objects, you can assign fields and values like this:

```js
object request_params = {
    method: "PATCH",
    headers: {
        Authorization: "Bearer ..."
    },
    body: {
        user: "Landon",
        data: {
            stuff: "..."
        }
    }
};
```

## Modules

You might've noticed that some of the examples contain `import "io"` or `import "math"`. These are
modules and they contain bindings to raw C code. The `upper` method of the `string` module looks
like this in C:

```c
struct MayValue* string_upper(struct MayValue** args, int arg_count) {
    char* upperstr = strdup(args[0]->as.string.val);

    for (int i = 0; upperstr[i] != '\0'; i++) {
        upperstr[i] = (char)toupper((unsigned char)upperstr[i]);
    }

    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_STRING;
    ret->as.string.val = upperstr;
    ret->as.string.len = strlen(upperstr);
    return ret;
}
```

## Web Development

Since we won't have access to React to build a bunch of AI slop web apps, I figured I would bring web development to May
with the http module. You can easily create an http web server to serve html to an end user:

```js
import "http";
import "io";

int server = create_server(8080);
print("Server running on port 8080");

while (1) {
    int client = accept_client(server);
    object req = read_request(client);
    print(req->method + " " + req->path);
    send_html(client, "<h1>Thank you for using MayLang!</h1>");
}
```

## Comments

Comments don't exist. This is because most of them are useless. If you write clean code, there is no need for comments anyway.
Additionally, this makes it difficult for AI to generate May code, as half of the code outputted by AI is comments.

If you really want comments for some reason, here are a few ways you could replicate them:

```js
import "io";

var comment(string com) {}

comment("Here we print hello world to the screen");
print("Hello world!");
```

Alternatively, you can just create a useless variable with a really long name:

```js
import "io";

var HERE_WE_PRINT_HELLO_WORLD;
print("Hello world!");
```

## Compatability

May should be able to run on every OS except Windows (purposeful decision).
