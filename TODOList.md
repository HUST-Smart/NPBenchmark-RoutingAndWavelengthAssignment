**1. 最小化总超载量(一条边上有10条业务，超载量记为9)**

### Notation

| Set  | Description | Size | Element | Remark |
| ---- | ----------- | ---- | ------- | ------ |
| $N$  | nodes       |      |         |        |
| $L$  | links       |      |         |        |
| $T$  | traffics    |      |         |        |

### Constant

| Constant      | Description                                                  | Type | Remark |
| ------------- | ------------------------------------------------------------ | ---- | ------ |
| $\Lambda_i^l$ | $\Lambda_i^l =1$ when node $i$ is the  origin node of link $l$  <br />$\Lambda_i^l =-1$ when node $i$ is the  terminal node of link $l$<br />$\Lambda_i^l =0$  otherwise. |      |        |
| $T_i^t$       | $T_i^t =1$ when node $i$ is the  origin node of  traffic $t$<br />$T_i^t =-1$ when node $i$ is the  terminal node of traffic $t$<br />$T_i^t =0$  otherwise. |      |        |

### Decision

| Variable | Description                                                  | Type   | Remark |
| -------- | ------------------------------------------------------------ | ------ | ------ |
| $v_l^t$  | equals 1 if traffic $t$ uses $l$ in the forward direction, and 0 otherwise | binary |        |
| $w_l^t$  | equals 1 if traffic $t$ uses $l$ in the backward direction, and 0 otherwise | binary |        |

### Constraint

(1) flow constraint
$$
\sum_{l=1}^{|L|} (\Lambda_i^lv_l^t-\Lambda_i^lw_l^t)= T_i^t, \forall i\in N,t \in T
$$
(2) overload constraint
$$
\sum_{t=1}^{|T|}v_l^t - c_l \le 1
$$

$$
\sum_{t=1}^{|T|}w_l^t - d_l \le 1
$$

$$
c_l \ge 0,d_l \ge 0, \forall l \in L
$$

### Objective

$$
min \quad F = \sum_{l=1}^{|L|}c_l+d_l
$$

**2. 使用拉格朗日松弛**

**3. 判定版本：给定总超载量及业务，判断能否在该超载量下将所有业务部署到网络**

**4. （*求出的解很可能不是最优解*）限制链路不能超载，使用模型最大化可容纳的业务数 ，剩余的业务使用最小化超载量（冲突业务数）进行计算，即可得到总超载量（总冲突业务数）** 