# Release Process

This document describes how to cut a new release of **Franz - Multi-Language Kafka Client Examples**.

---

## 1. Prepare the Release

1. Ensure your branch is up to date with `main`:
    ```bash
    git checkout main
    git pull origin main
    ```
2. Update `CHANGELOG.md`:
    - Move items from the [Unreleased] section into a new version section.
    - Use the format:
    ```css
    ## [X.Y.Z] - YYYY-MM-DD
    ```
    - Summarize major changes under `### Added`, `### Changed`, `### Fixed`, etc.
3. Update any version strings in:
    - Documentation (`README.md`, setup instructions).
    - Kubernetes manifests if versioned.
    - Example configs if needed.


## 2. Tag the Release

1. Commit the updated changelog and version bump:
    ```bash
    git add CHANGELOG.md
    git commit -m "chore(release): prepare vX.Y.Z"
    ```
2. Create a signed Git tag:
    ```bash
    git tag -s vX.Y.Z -m "Franz vX.Y.Z"
    ```
3. Push commits and tags:
    ```bash
    git push origin main
    git push origin vX.Y.Z
    ```


## 3. Create GitHub Release

1. Go to the [GitHub Releases](../../releases) page.
2. Click **Draft a new release**.
3. Select tag `vX.Y.Z`.
4. Title: `Franz vX.Y.Z`
5. Description:
    - Paste the relevant section from `CHANGELOG.md`.
    - Include highlights and upgrade notes.
6. Publish the release.


## 4. Post-Release

- Update the **[Unreleased]** section in `CHANGELOG.md` for future changes:
    ```markdown
    ## [Unreleased]
    - Pending changes
    ```
- Announce release in team channels or documentation.


## 5. Versioning Policy

- Follows [Semantic Versioning](https://semver.org/).
- `X.Y.Z`:
    - `X` = breaking changes
    - `Y` = new features (backward compatible)
    - `Z` = bug fixes and small improvements

---