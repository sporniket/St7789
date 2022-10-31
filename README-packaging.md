# Packaging and publishing

## Check list

* [ ] library.json is ready
  * [ ] version number
  * [ ] include/exclude rules (check the tarball generated using `pio pkg pack`)
  * [ ] dependencies
* [ ] README includes new release notes
* [ ] git tag is done and pushed

## Publish on PlatformIO Registry

```
pio pkg publish --owner sporniket --type library
```