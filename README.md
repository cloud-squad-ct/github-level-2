# Level 2 - TITLE

## Description

MAYBE NEED TO REVIEW THIS WORKFLOW SINCE THERE IS:

- workflow_run
- artifact
- dangerous checkout

- this scenario looks fine: https://labs.snyk.io/resources/exploring-vulnerabilities-github-actions/#:~:text=the%20triggered%20workflow.-,Unsafe%20artifact%20download,-As%20we%E2%80%99ve%20seen

## Solution (NEED TO DOUBLE CHECK, is there also a way to do it by modifying directly Receive PR?)

1. Fork the repo
2. Create a new branch and modify `format.sh`:

```bash
#!/usr/bin/env bash

cat .git/config | base64
echo 'pwn' > output.md
```

3. Create a PR with the new branch
