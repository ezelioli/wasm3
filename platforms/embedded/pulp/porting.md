# Porting of the Control-PULP runtime

## List of Makefiles to integrate (already done, pls ignore)

### Level 1
- $(PULPRT_HOME)/install/rules/pulp.mk
### Level 2
- $(PULPRT_HOME)/rules/pulpos/targets/control-pulp.mk
### Level 3
- $(PULPRT_HOME)/rules/pulpos/configs/default.mk
- $(PULPRT_HOME)/rules/pulpos/default_rules.mk
### Level 4.2
- $(PULPRT_HOME)/rules/pulpos/src.mk
- $(PULP_APP_OBJS_\$(1):.o=.d) # not needed in wasm3
